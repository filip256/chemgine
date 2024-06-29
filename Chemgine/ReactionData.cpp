#include "ReactionData.hpp"
#include "SystemMatrix.hpp"
#include "RetrosynthReaction.hpp"
#include "Maths.hpp"
#include "PairHash.hpp"
#include "Logger.hpp"
#include "Utils.hpp"

ReactionData::ReactionData(
	const ReactionId id,
	const std::string& name,
	const std::vector<std::pair<Reactable, uint8_t>>& reactants,
	const std::vector<std::pair<Reactable, uint8_t>>& products,
	const Amount<Unit::MOLE_PER_SECOND> baseSpeed,
	const Amount<Unit::CELSIUS> baseTemperature,
	const Amount<Unit::JOULE_PER_MOLE> reactionEnergy,
	const Amount<Unit::JOULE_PER_MOLE> activationEnergy,
	ImmutableSet<Catalyst>&& catalysts
) noexcept :
	id(id),
	baseSpeed(baseSpeed),
	baseTemperature(baseTemperature),
	reactionEnergy(reactionEnergy),
	activationEnergy(activationEnergy),
	name(name),
	reactants(Utils::flatten<Reactable, uint8_t>(reactants)),
	products(Utils::flatten<Reactable, uint8_t>(products)),
	catalysts(std::move(catalysts))
{}

bool ReactionData::balance(
	std::vector<std::pair<Reactable, uint8_t>>& reactants,
	std::vector<std::pair<Reactable, uint8_t>>& products)
{
	if (reactants.size() == 0 || products.size() == 0)
		return false;

	SystemMatrix<float> system;
	const size_t syslen = reactants.size() + products.size() - 1;
	std::unordered_map<AtomId, size_t> sysmap;

	for (size_t i = 0; i < reactants.size(); ++i)
	{
		const auto map = reactants[i].first.getStructure().getComponentCountMap();
		for (const auto& c : map)
		{
			if (sysmap.contains(c.first) == false)
			{
				sysmap.emplace(std::move(std::make_pair(c.first, sysmap.size())));
				system.addNullRow(syslen);
				system.back()[i] = static_cast<float>(c.second);
			}
			else
				system[sysmap[c.first]][i] = static_cast<float>(c.second);
		}
	}

	// the "1st product" rule: lock the coefficient to 1 and apply to system
	const auto map = products[0].first.getStructure().getComponentCountMap();
	for (const auto& c : map)
	{
		if (sysmap.contains(c.first) == false)
		{
			sysmap.emplace(std::move(std::make_pair(c.first, sysmap.size())));
			system.addNullRow(syslen);
			system.back().back() = static_cast<float>(c.second);
		}
		else
			system[sysmap[c.first]].back() = static_cast<float>(c.second);
	}

	for (size_t i = 1; i < products.size(); ++i)
	{
		const auto map = products[i].first.getStructure().getComponentCountMap();
		for (const auto& c : map)
		{
			if (sysmap.contains(c.first) == false)
			{
				sysmap.emplace(std::move(std::make_pair(c.first, sysmap.size())));
				system.addNullRow(syslen);
				system.back()[reactants.size() + i - 1] = -1 * static_cast<float>(c.second);
			}
			else
				system[sysmap[c.first]][reactants.size() + i - 1] = -1 * static_cast<float>(c.second);
		}
	}


	// TODO: The most complex product should be fixed, not the first.
	const auto result = system.solve();
	if (result.empty())
		return false;

	const auto intCoef = Maths::integerCoefficient(result);

	for (size_t i = 0; i < reactants.size(); ++i)
		reactants[i].second = std::roundf(result[i] * intCoef);
	products[0].second = intCoef;
	for (size_t i = 1; i < products.size(); ++i)
		products[i].second = std::roundf(result[reactants.size() + i - 1] *intCoef);

	return true;
}

bool ReactionData::mapReactantsToProducts()
{
	componentMapping.clear();
	if (reactants.size() == 0 || products.size() == 0)
		return false;

	componentMapping.reserve(reactants.size() + products.size());
	std::vector<std::unordered_set<c_size>> reactantIgnore(reactants.size()), productIgnore(products.size());

	for (size_t i = 0; i < reactants.size(); ++i)
	{
		if (reactants[i].getStructure().isVirtualHydrogen())
			continue;

		std::pair<std::unordered_map<c_size, c_size>, uint8_t> maxMap;
		size_t maxIdxJ = 0;
		for (size_t j = 0; j < products.size(); ++j)
		{
			const auto map = reactants[i].getStructure().maximalMapTo(products[j].getStructure(), reactantIgnore[i], productIgnore[j]);
			if (map.first.size() > maxMap.first.size() ||
				(map.first.size() == maxMap.first.size() && map.second > maxMap.second))
			{
				maxMap = map;
				maxIdxJ = j;
			}
		}

		if (maxMap.first.size() == 0)
			return false;

		for (const auto& p : maxMap.first)
		{
			reactantIgnore[i].insert(p.first);
			productIgnore[maxIdxJ].insert(p.second);

			// only save radical atoms
			if (reactants[i].getStructure().getAtom(p.first)->isRadical())
				componentMapping.emplace(std::make_pair(std::make_pair(i, p.first), std::make_pair(maxIdxJ, p.second)));
		}

		if (reactants[i].getStructure().componentCount() != reactantIgnore[i].size())
			--i;
	}

	// check if mapping is complete
	for (size_t i = 0; i < reactants.size(); ++i)
	{
		if (reactants[i].getStructure().componentCount() != reactantIgnore[i].size())
		{
			return false;
		}
	}
	for (size_t i = 0; i < products.size(); ++i)
	{
		if (products[i].getStructure().componentCount() != productIgnore[i].size())
		{
			return false;
		}
	}

	return true;

}

bool ReactionData::hasAsReactant(const Reactant& reactant) const
{
	for (size_t i = 0; i < reactants.size(); ++i)
	{
		if (reactant.molecule.getStructure().mapTo(reactants[i].getStructure(), true).size() != 0)
			return true;
	}
	return false;
}

std::vector<std::unordered_map<c_size, c_size>> ReactionData::generateConcreteReactantMatches(
	const std::vector<Reactant>& molecules) const
{
	if (reactants.size() != molecules.size())
		return {};

	// find the molecule match for each reactant
	std::vector<std::unordered_map<c_size, c_size>> matches;
	matches.reserve(reactants.size());
	for (size_t i = 0; i < reactants.size(); ++i)
	{
		if (reactants[i].getStructure().isVirtualHydrogen() && molecules[i].molecule.getStructure().isVirtualHydrogen())
		{
			matches.emplace_back();
			continue;
		}

		matches.emplace_back(Utils::reverseMap(reactants[i].matchWith(molecules[i].molecule.getStructure())));
		if (matches.back().empty())
			return {};
	}
	return matches;
}

std::pair<size_t, std::unordered_map<c_size, c_size>> ReactionData::generateRetrosynthProductMatches(
	const Reactable& targetProduct) const
{
	for (size_t i = 0; i < products.size(); ++i)
	{
		if (products[i].getStructure().isVirtualHydrogen() && targetProduct.getStructure().isVirtualHydrogen())
			return std::make_pair(i, std::unordered_map<c_size, c_size>());

		auto match = Utils::reverseMap(products[i].matchWith(targetProduct));
		if(match.size())
			return std::make_pair(i, std::move(match));
	}

	return std::make_pair(npos, std::unordered_map<c_size, c_size>());
}

std::vector<Molecule> ReactionData::generateConcreteProducts(const std::vector<Reactant>& molecules,
	const std::vector<std::unordered_map<c_size, c_size>>& matches) const
{
	if (matches.size() != reactants.size())
		return std::vector<Molecule>();

	// build concrete products
	std::vector<MolecularStructure> concreteProducts;
	concreteProducts.reserve(products.size());
	for (size_t i = 0; i < products.size(); ++i)
		concreteProducts.emplace_back(std::move(products[i].getStructure().createCopy()));

	for (const auto& p : componentMapping)
	{
		std::unordered_map<c_size, c_size> tempMap = { {matches[p.first.first].at(p.first.second), p.second.second}};
		MolecularStructure::copyBranch(
			concreteProducts[p.second.first],
			molecules[p.first.first].molecule.getStructure(),
			matches[p.first.first].at(p.first.second),
			tempMap,
			false,
			Utils::extractUniqueValues(matches[p.first.first])
		);
	}

	// canonicalize
	std::vector<Molecule> result;
	result.reserve(concreteProducts.size());
	for (size_t i = 0; i < concreteProducts.size(); ++i)
	{
		concreteProducts[i].canonicalize();
		concreteProducts[i].recountImpliedHydrogens();
		result.emplace_back(std::move(concreteProducts[i]));
	}
	return result;
}

RetrosynthReaction ReactionData::generateRetrosynthReaction(
	const Reactable& targetProduct,
	const std::pair<size_t, std::unordered_map<c_size, c_size>>& match) const
{
	// build concrete products
	std::vector<MolecularStructure> substReactants;
	substReactants.reserve(reactants.size());
	for (size_t i = 0; i < reactants.size(); ++i)
		substReactants.emplace_back(std::move(reactants[i].getStructure().createCopy()));

	const auto targetMatchedComponents = Utils::extractUniqueValues(match.second);
	const auto reversedMapping = Utils::reverseMap(componentMapping);
	for (const auto& p : reversedMapping)
	{
		if (p.first.first != match.first)
			continue;

		std::unordered_map<c_size, c_size> tempMap = { {match.second.at(p.first.second), p.second.second} };
		MolecularStructure::copyBranch(
			substReactants[p.second.first],
			targetProduct.getStructure(),
			match.second.at(p.first.second),
			tempMap,
			false,
			targetMatchedComponents
		);
	}

	// canonicalize reactants
	std::vector<Reactable> reactantReactables;
	reactantReactables.reserve(substReactants.size());
	for (size_t i = 0; i < substReactants.size(); ++i)
	{
		substReactants[i].canonicalize();
		substReactants[i].recountImpliedHydrogens();
		reactantReactables.emplace_back(*Reactable::get(std::move(substReactants[i])));
	}

	// copy products
	std::vector<Reactable> productReactables;
	productReactables.reserve(products.size());
	productReactables.emplace_back(targetProduct);
	for (size_t i = 0; i < products.size(); ++i)
		if (i != match.first)
			productReactables.emplace_back(products[i]);

	return RetrosynthReaction(*this, reactantReactables, productReactables);
}

const std::vector<Reactable>& ReactionData::getReactants() const
{
	return reactants;
}

const std::vector<Reactable>& ReactionData::getProducts() const
{
	return products;
}

const ImmutableSet<Catalyst> &ReactionData::getCatalysts() const
{
	return catalysts;
}

bool ReactionData::isCutReaction() const
{
	return baseSpeed == 0.0;
}

bool ReactionData::isSpecializationOf(const ReactionData& other) const
{
	// check reactants
	std::vector<uint8_t> used(other.reactants.size(), false);
	for (size_t i = 0; i < this->reactants.size(); ++i)
	{
		bool matchFound = false;
		for (size_t j = 0; j < other.reactants.size(); ++j)
		{
			if (used[j])
				continue;

			if (other.reactants[j].matchesWith(this->reactants[i]))
			{
				matchFound = true;
				used[j] = true;
				break;
			}
		}

		if (matchFound == false)
			return false;
	}

	// check products
	used = std::vector<uint8_t>(other.products.size(), false);
	for (size_t i = 0; i < this->products.size(); ++i)
	{
		bool matchFound = false;
		for (size_t j = 0; j < other.products.size(); ++j)
		{
			if (used[j])
				continue;

			if (other.products[j].matchesWith(this->products[i]))
			{
				matchFound = true;
				used[j] = true;
				break;
			}
		}

		if (matchFound == false)
			return false;
	}

	// check catalysts
	used = std::vector<uint8_t>(this->catalysts.size(), false);
	for (size_t i = 0; i < other.catalysts.size(); ++i)
	{
		bool matchFound = false;
		for (size_t j = 0; j < this->catalysts.size(); ++j)
		{
			if (used[j])
				continue;

			if (other.catalysts[i].matchesWith(this->catalysts[j]))
			{
				matchFound = true;
				used[j] = true;
				break;
			}
		}

		if (matchFound == false)
			return false;
	}

	return true;
}

bool ReactionData::isGeneralizationOf(const ReactionData& other) const
{
	return other.isSpecializationOf(*this);
}

bool ReactionData::isEquivalentTo(const ReactionData& other) const
{
	if (this->reactants.size() != other.reactants.size() ||
		this->products.size() != other.products.size() ||
		this->catalysts.size() != other.catalysts.size())
		return false;

	// check reactants
	std::vector<uint8_t> used(other.reactants.size(), false);
	for (size_t i = 0; i < this->reactants.size(); ++i)
	{
		bool matchFound = false;
		for (size_t j = 0; j < other.reactants.size(); ++j)
		{
			if (used[j])
				continue;

			if (other.reactants[j] == this->reactants[i])
			{
				matchFound = true;
				used[j] = true;
				break;
			}
		}

		if (matchFound == false)
			return false;
	}

	// check products
	used = std::vector<uint8_t>(other.products.size(), false);
	for (size_t i = 0; i < this->products.size(); ++i)
	{
		bool matchFound = false;
		for (size_t j = 0; j < other.products.size(); ++j)
		{
			if (used[j])
				continue;

			if (other.products[j] == this->products[i])
			{
				matchFound = true;
				used[j] = true;
				break;
			}
		}

		if (matchFound == false)
			return false;
	}


	// check catalysts
	used = std::vector<uint8_t>(this->catalysts.size(), false);
	for (size_t i = 0; i < other.catalysts.size(); ++i)
	{
		bool matchFound = false;
		for (size_t j = 0; j < this->catalysts.size(); ++j)
		{
			if (used[j])
				continue;

			if (other.catalysts[i] == this->catalysts[j])
			{
				matchFound = true;
				used[j] = true;
				break;
			}
		}

		if (matchFound == false)
			return false;
	}

	return true;
}

void ReactionData::setBaseReaction(const ReactionData& reaction)
{
	baseReaction = reaction;
}

std::string ReactionData::getHRTag() const
{
	return '<' + std::to_string(id) + ':' + name + '>';
}
