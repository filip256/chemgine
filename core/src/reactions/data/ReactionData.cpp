#include "reactions/data/ReactionData.hpp"

#include "data/def/DataDumper.hpp"
#include "io/Log.hpp"
#include "reactions/ReactionSpecifier.hpp"
#include "reactions/kinds/RetrosynthReaction.hpp"
#include "structs/SystemMatrix.hpp"
#include "utils/Hash.hpp"
#include "utils/Math.hpp"

ReactionData::ReactionData(
    const ReactionId                                     id,
    const std::string&                                   name,
    const std::vector<std::pair<StructureRef, uint8_t>>& reactants,
    const std::vector<std::pair<StructureRef, uint8_t>>& products,
    const Amount<Unit::JOULE_PER_MOLE>                   reactionEnergy,
    const Amount<Unit::JOULE_PER_MOLE>                   activationEnergy,
    EstimatorRef<Unit::MOLE_PER_SECOND, Unit::CELSIUS>&& tempSpeedEstimator,
    EstimatorRef<Unit::NONE, Unit::MOLE_RATIO>&&         concSpeedEstimator,
    ImmutableSet<Catalyst>&&                             catalysts) noexcept :
    id(id),
    isCut(false),
    reactionEnergy(reactionEnergy),
    activationEnergy(activationEnergy),
    name(name),
    tempSpeedEstimator(std::move(tempSpeedEstimator)),
    concSpeedEstimator(std::move(concSpeedEstimator)),
    reactants(utils::flatten<StructureRef, uint8_t>(reactants)),
    products(utils::flatten<StructureRef, uint8_t>(products)),
    catalysts(std::move(catalysts))
{}

ReactionData::ReactionData(
    const ReactionId                                     id,
    const std::string&                                   name,
    const std::vector<std::pair<StructureRef, uint8_t>>& reactants,
    const std::vector<std::pair<StructureRef, uint8_t>>& products,
    EstimatorRef<Unit::MOLE_PER_SECOND, Unit::CELSIUS>&& tempSpeedEstimator,
    EstimatorRef<Unit::NONE, Unit::MOLE_RATIO>&&         concSpeedEstimator,
    ImmutableSet<Catalyst>&&                             catalysts) noexcept :
    id(id),
    isCut(true),
    name(name),
    tempSpeedEstimator(std::move(tempSpeedEstimator)),
    concSpeedEstimator(std::move(concSpeedEstimator)),
    reactants(utils::flatten<StructureRef, uint8_t>(reactants)),
    products(utils::flatten<StructureRef, uint8_t>(products)),
    catalysts(std::move(catalysts))
{}

bool ReactionData::balance(
    std::vector<std::pair<StructureRef, uint8_t>>& reactants, std::vector<std::pair<StructureRef, uint8_t>>& products)
{
    if (reactants.size() == 0 || products.size() == 0)
        return false;

    SystemMatrix<float_s>              system;
    const size_t                       syslen = reactants.size() + products.size() - 1;
    std::unordered_map<Symbol, size_t> sysmap;

    for (size_t i = 0; i < reactants.size(); ++i) {
        const auto map = reactants[i].first.getStructure().getComponentCountMap();
        for (const auto& c : map) {
            if (sysmap.contains(c.first) == false) {
                sysmap.emplace(c.first, sysmap.size());
                system.addNullRow(syslen);
                system.back()[i] = static_cast<float_s>(c.second);
            }
            else
                system[sysmap[c.first]][i] = static_cast<float_s>(c.second);
        }
    }

    // the "1st product" rule: lock the coefficient to 1 and apply to system
    const auto map = products[0].first.getStructure().getComponentCountMap();
    for (const auto& c : map) {
        if (sysmap.contains(c.first) == false) {
            sysmap.emplace(c.first, sysmap.size());
            system.addNullRow(syslen);
            system.back().back() = static_cast<float_s>(c.second);
        }
        else
            system[sysmap[c.first]].back() = static_cast<float_s>(c.second);
    }

    for (size_t i = 1; i < products.size(); ++i) {
        const auto map = products[i].first.getStructure().getComponentCountMap();
        for (const auto& c : map) {
            if (sysmap.contains(c.first) == false) {
                sysmap.emplace(c.first, sysmap.size());
                system.addNullRow(syslen);
                system.back()[reactants.size() + i - 1] = -1 * static_cast<float_s>(c.second);
            }
            else
                system[sysmap[c.first]][reactants.size() + i - 1] = -1 * static_cast<float_s>(c.second);
        }
    }

    // TODO: The most complex product should be fixed, not the first.
    const auto result = system.solve();
    if (result.empty())
        return false;

    const auto intCoef = utils::integerCoefficient(result);

    for (size_t i = 0; i < reactants.size(); ++i) reactants[i].second = round_cast<uint8_t>(result[i] * intCoef);
    products[0].second = intCoef;
    for (size_t i = 1; i < products.size(); ++i)
        products[i].second = round_cast<uint8_t>(result[reactants.size() + i - 1] * intCoef);

    return true;
}

bool ReactionData::mapReactantsToProducts()
{
    componentMapping.clear();
    if (reactants.size() == 0 || products.size() == 0)
        return false;

    componentMapping.reserve(reactants.size() + products.size());
    std::vector<std::unordered_set<c_size>> reactantIgnore(reactants.size()), productIgnore(products.size());

    for (size_t i = 0; i < reactants.size(); ++i) {
        if (reactants[i].getStructure().isVirtualHydrogen())
            continue;

        std::pair<std::unordered_map<c_size, c_size>, uint8_t> maxMap;
        size_t                                                 maxIdxJ = 0;
        for (size_t j = 0; j < products.size(); ++j) {
            const auto map =
                reactants[i].getStructure().maximalMapTo(products[j].getStructure(), reactantIgnore[i], productIgnore[j]);
            if (map.first.size() > maxMap.first.size() ||
                (map.first.size() == maxMap.first.size() && map.second > maxMap.second)) {
                maxMap  = map;
                maxIdxJ = j;
            }
        }

        if (maxMap.first.size() == 0)
            return false;

        for (const auto& p : maxMap.first) {
            reactantIgnore[i].insert(p.first);
            productIgnore[maxIdxJ].insert(p.second);

            // only save radical atoms
            if (reactants[i].getStructure().getAtom(p.first).isRadical())
                componentMapping.emplace(std::make_pair(std::make_pair(i, p.first), std::make_pair(maxIdxJ, p.second)));
        }

        if (reactants[i].getStructure().getNonImpliedAtomCount() != reactantIgnore[i].size())
            --i;
    }

    // check if mapping is complete
    for (size_t i = 0; i < reactants.size(); ++i) {
        if (reactants[i].getStructure().getNonImpliedAtomCount() != reactantIgnore[i].size()) {
            return false;
        }
    }
    for (size_t i = 0; i < products.size(); ++i) {
        if (products[i].getStructure().getNonImpliedAtomCount() != productIgnore[i].size()) {
            return false;
        }
    }

    return true;
}

bool ReactionData::hasAsReactant(const MolecularStructure& structure) const
{
    for (size_t i = 0; i < reactants.size(); ++i) {
        if (structure.mapTo(reactants[i].getStructure(), true).size() != 0)
            return true;
    }
    return false;
}

bool ReactionData::hasAsReactant(const Molecule& molecule) const { return hasAsReactant(molecule.getStructure()); }

std::vector<std::unordered_map<c_size, c_size>>
ReactionData::generateConcreteReactantMatches(const std::vector<Reactant>& molecules) const
{
    if (reactants.size() != molecules.size())
        return {};

    // find the molecule match for each reactant
    std::vector<std::unordered_map<c_size, c_size>> matches;
    matches.reserve(reactants.size());
    for (size_t i = 0; i < reactants.size(); ++i) {
        if (reactants[i].getStructure().isVirtualHydrogen() && molecules[i].molecule.getStructure().isVirtualHydrogen()) {
            matches.emplace_back();
            continue;
        }

        matches.emplace_back(utils::reverseMap(reactants[i].matchWith(molecules[i].molecule.getStructure())));
        if (matches.back().empty())
            return {};
    }
    return matches;
}

std::pair<size_t, std::unordered_map<c_size, c_size>>
ReactionData::generateRetrosynthProductMatches(const StructureRef& targetProduct) const
{
    for (size_t i = 0; i < products.size(); ++i) {
        if (products[i].getStructure().isVirtualHydrogen() && targetProduct.getStructure().isVirtualHydrogen())
            return std::make_pair(i, std::unordered_map<c_size, c_size>());

        auto match = utils::reverseMap(products[i].matchWith(targetProduct));
        if (match.size())
            return std::make_pair(i, std::move(match));
    }

    return std::make_pair(npos, std::unordered_map<c_size, c_size>());
}

std::vector<Molecule> ReactionData::generateConcreteProducts(
    const std::vector<Reactant>& molecules, const std::vector<std::unordered_map<c_size, c_size>>& matches) const
{
    if (matches.size() != reactants.size())
        return std::vector<Molecule>();

    // build concrete products
    std::vector<MolecularStructure> concreteProducts;
    concreteProducts.reserve(products.size());
    for (size_t i = 0; i < products.size(); ++i) concreteProducts.emplace_back(products[i].getStructure().createCopy());

    for (const auto& p : componentMapping) {
        std::unordered_map<c_size, c_size> tempMap = {
            {matches[p.first.first].at(p.first.second), p.second.second}
        };
        MolecularStructure::copyBranch(
            concreteProducts[p.second.first],
            molecules[p.first.first].molecule.getStructure(),
            matches[p.first.first].at(p.first.second),
            tempMap,
            false,
            utils::extractUniqueValues(matches[p.first.first]));
    }

    // canonicalize
    std::vector<Molecule> result;
    result.reserve(concreteProducts.size());
    for (size_t i = 0; i < concreteProducts.size(); ++i) {
        concreteProducts[i].canonicalize();
        concreteProducts[i].recountImpliedHydrogens();

        // TODO: add polymer support
        if (concreteProducts[i].getNonImpliedAtomCount() > 100)
            continue;

        result.emplace_back(std::move(concreteProducts[i]));
    }
    return result;
}

RetrosynthReaction ReactionData::generateRetrosynthReaction(
    const StructureRef& targetProduct, const std::pair<size_t, std::unordered_map<c_size, c_size>>& match) const
{
    // build concrete products
    std::vector<MolecularStructure> substReactants;
    substReactants.reserve(reactants.size());
    for (size_t i = 0; i < reactants.size(); ++i) substReactants.emplace_back(reactants[i].getStructure().createCopy());

    const auto targetMatchedComponents = utils::extractUniqueValues(match.second);
    const auto reversedMapping         = utils::reverseMap(componentMapping);
    for (const auto& p : reversedMapping) {
        if (p.first.first != match.first)
            continue;

        std::unordered_map<c_size, c_size> tempMap = {
            {match.second.at(p.first.second), p.second.second}
        };
        MolecularStructure::copyBranch(
            substReactants[p.second.first],
            targetProduct.getStructure(),
            match.second.at(p.first.second),
            tempMap,
            false,
            targetMatchedComponents);
    }

    // canonicalize reactants
    std::vector<StructureRef> reactantStructureRefs;
    reactantStructureRefs.reserve(substReactants.size());
    for (size_t i = 0; i < substReactants.size(); ++i) {
        substReactants[i].canonicalize();
        substReactants[i].recountImpliedHydrogens();
        reactantStructureRefs.emplace_back(*StructureRef::create(std::move(substReactants[i])));
    }

    // copy products
    std::vector<StructureRef> productStructureRefs;
    productStructureRefs.reserve(products.size());
    productStructureRefs.emplace_back(targetProduct);
    for (size_t i = 0; i < products.size(); ++i)
        if (i != match.first)
            productStructureRefs.emplace_back(products[i]);

    return RetrosynthReaction(*this, reactantStructureRefs, productStructureRefs);
}

const std::vector<StructureRef>& ReactionData::getReactants() const { return reactants; }

const std::vector<StructureRef>& ReactionData::getProducts() const { return products; }

const ImmutableSet<Catalyst>& ReactionData::getCatalysts() const { return catalysts; }

Amount<Unit::MOLE_PER_SECOND>
ReactionData::getSpeedAt(const Amount<Unit::CELSIUS> temperature, const Amount<Unit::MOLE_RATIO> concentration) const
{
    return tempSpeedEstimator->get(temperature) * concSpeedEstimator->get(concentration).asStd();
}

bool ReactionData::isCutReaction() const { return isCut; }

bool ReactionData::isSpecializationOf(const ReactionData& other) const
{
    // check reactants
    std::vector<uint8_t> used(other.reactants.size(), false);
    for (size_t i = 0; i < this->reactants.size(); ++i) {
        bool matchFound = false;
        for (size_t j = 0; j < other.reactants.size(); ++j) {
            if (used[j])
                continue;

            if (other.reactants[j].matchesWith(this->reactants[i])) {
                matchFound = true;
                used[j]    = true;
                break;
            }
        }

        if (matchFound == false)
            return false;
    }

    // check products
    used = std::vector<uint8_t>(other.products.size(), false);
    for (size_t i = 0; i < this->products.size(); ++i) {
        bool matchFound = false;
        for (size_t j = 0; j < other.products.size(); ++j) {
            if (used[j])
                continue;

            if (other.products[j].matchesWith(this->products[i])) {
                matchFound = true;
                used[j]    = true;
                break;
            }
        }

        if (matchFound == false)
            return false;
    }

    // check catalysts
    used = std::vector<uint8_t>(this->catalysts.size(), false);
    for (size_t i = 0; i < other.catalysts.size(); ++i) {
        bool matchFound = false;
        for (size_t j = 0; j < this->catalysts.size(); ++j) {
            if (used[j])
                continue;

            if (other.catalysts[i].matchesWith(this->catalysts[j])) {
                matchFound = true;
                used[j]    = true;
                break;
            }
        }

        if (matchFound == false)
            return false;
    }

    return true;
}

bool ReactionData::isGeneralizationOf(const ReactionData& other) const { return other.isSpecializationOf(*this); }

bool ReactionData::isEquivalentTo(const ReactionData& other) const
{
    if (this->reactants.size() != other.reactants.size() ||
        this->products.size() != other.products.size() ||
        this->catalysts.size() != other.catalysts.size())
        return false;

    // check reactants
    std::vector<uint8_t> used(other.reactants.size(), false);
    for (size_t i = 0; i < this->reactants.size(); ++i) {
        bool matchFound = false;
        for (size_t j = 0; j < other.reactants.size(); ++j) {
            if (used[j])
                continue;

            if (other.reactants[j] == this->reactants[i]) {
                matchFound = true;
                used[j]    = true;
                break;
            }
        }

        if (matchFound == false)
            return false;
    }

    // check products
    used = std::vector<uint8_t>(other.products.size(), false);
    for (size_t i = 0; i < this->products.size(); ++i) {
        bool matchFound = false;
        for (size_t j = 0; j < other.products.size(); ++j) {
            if (used[j])
                continue;

            if (other.products[j] == this->products[i]) {
                matchFound = true;
                used[j]    = true;
                break;
            }
        }

        if (matchFound == false)
            return false;
    }

    // check catalysts
    used = std::vector<uint8_t>(this->catalysts.size(), false);
    for (size_t i = 0; i < other.catalysts.size(); ++i) {
        bool matchFound = false;
        for (size_t j = 0; j < this->catalysts.size(); ++j) {
            if (used[j])
                continue;

            if (other.catalysts[i] == this->catalysts[j]) {
                matchFound = true;
                used[j]    = true;
                break;
            }
        }

        if (matchFound == false)
            return false;
    }

    return true;
}

void ReactionData::setBaseReaction(const ReactionData& reaction) { baseReaction = reaction; }

std::string ReactionData::getHRTag() const { return '<' + std::to_string(id) + ':' + name + '>'; }

void
ReactionData::dumpDefinition(std::ostream& out, const bool prettify, std::unordered_set<EstimatorId>& alreadyPrinted) const
{
    static const auto valueOffset = checked_cast<uint8_t>(utils::max(
        def::Reactions::Id.size(),
        def::Reactions::Name.size(),
        def::Reactions::Catalysts.size(),
        def::Reactions::Energy.size(),
        def::Reactions::Activation.size(),
        def::Reactions::TemperatureSpeed.size(),
        def::Reactions::ConcentrationSpeed.size()));

    const auto compare         = [](const StructureRef& l, const StructureRef& r) { return l.getId() < r.getId(); };
    const auto uniqueReactants = ImmutableSet<StructureRef>::toSortedSetVector(utils::copy(reactants), compare);
    const auto uniqueProducts  = ImmutableSet<StructureRef>::toSortedSetVector(utils::copy(products), compare);

    std::vector<std::string> reactantsStr;
    std::vector<std::string> productsStr;
    reactantsStr.reserve(uniqueReactants.size());
    productsStr.reserve(uniqueProducts.size());
    for (size_t i = 0; i < uniqueReactants.size(); ++i)
        reactantsStr.emplace_back(def::print(uniqueReactants[i].getStructure()));
    for (size_t i = 0; i < uniqueProducts.size(); ++i)
        productsStr.emplace_back(def::print(uniqueProducts[i].getStructure()));

    def::DataDumper dump(out, valueOffset, 0, prettify);
    dump.tryOolSubDefinition(tempSpeedEstimator, alreadyPrinted)
        .tryOolSubDefinition(concSpeedEstimator, alreadyPrinted)
        .header(def::Types::Reaction, def::ReactionSpecifier(std::move(reactantsStr), std::move(productsStr)), "")
        .beginProperties()
        .propertyWithSep(def::Reactions::Id, id)
        .propertyWithSep(def::Reactions::Name, name);

    if (catalysts.size())
        dump.propertyWithSep(def::Reactions::Catalysts, catalysts);

    if (isCut) {
        dump.property(def::Reactions::IsCut, true).endProperties().endDefinition();
        return;
    }

    dump.defaultPropertyWithSep(def::Reactions::Energy, reactionEnergy, Amount<Unit::JOULE_PER_MOLE>(0.0))
        .defaultPropertyWithSep(def::Reactions::Activation, activationEnergy, Amount<Unit::JOULE_PER_MOLE>(0.0))
        .subDefinitionWithSep(def::Reactions::TemperatureSpeed, tempSpeedEstimator, alreadyPrinted)
        .subDefinition(def::Reactions::ConcentrationSpeed, concSpeedEstimator, alreadyPrinted)
        .endProperties()
        .endDefinition();
}

void ReactionData::print(std::ostream& out) const
{
    std::unordered_set<EstimatorId> history;
    dumpDefinition(out, true, history);
}
