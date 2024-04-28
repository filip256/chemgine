#include "ReactionData.hpp"
#include "SystemMatrix.hpp"
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
	std::vector<Catalyst>&& catalysts
) noexcept :
	id(id),
	baseSpeed(baseSpeed),
	baseTemperature(baseTemperature),
	reactionEnergy(reactionEnergy),
	activationEnergy(activationEnergy),
	name(name),
	reactants(flatten(reactants)),
	products(flatten(products)),
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
	std::unordered_map<ComponentId, size_t> sysmap;

	for (size_t i = 0; i < reactants.size(); ++i)
	{
		const auto map = reactants[i].first.getStructure().getComponentCountMap();
		for (const auto& c : map)
		{
			if (sysmap.contains(c.first) == false)
			{
				sysmap.emplace(std::move(std::make_pair(c.first, sysmap.size())));
				system.addRow(syslen);
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
			system.addRow(syslen);
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
				system.addRow(syslen);
				system.back()[reactants.size() + i - 1] = -1 * static_cast<float>(c.second);
			}
			else
				system[sysmap[c.first]][reactants.size() + i - 1] = -1 * static_cast<float>(c.second);
		}
	}


	//The most complex product should be fixed, not the first.
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
			if (reactants[i].getStructure().getComponent(p.first)->isRadicalType())
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

std::vector<Reactable> ReactionData::flatten(
	const std::vector<std::pair<Reactable, uint8_t>>& list)
{
	std::vector<Reactable> result;
	result.reserve(list.size());

	for (size_t i = 0; i < list.size(); ++i)
		for(uint8_t j = 0; j < list[i].second; ++j)
			result.emplace_back(list[i].first);

	return result;
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

void ReactionData::enumerateReactantPairs(
	const std::vector<Molecule>& molecules,
	const std::unordered_set<std::pair<size_t, size_t>>& allowedPairs,
	std::vector<std::pair<size_t, size_t>>& currentMatch,
	std::vector<std::vector<std::pair<size_t, size_t>>>& result) const
{
	if (currentMatch.size() == reactants.size()) {
		result.emplace_back(std::move(currentMatch));
		return;
	}

	for (size_t i = 0; i < molecules.size(); ++i) {
		const auto potentialPair = std::make_pair(i, currentMatch.size());

		if (allowedPairs.contains(potentialPair)) 
		{
			currentMatch.push_back(potentialPair);
			enumerateReactantPairs(molecules, allowedPairs, currentMatch, result);

			if(currentMatch.size())
				currentMatch.pop_back();
		}
	}
}

std::vector<std::vector<std::pair<size_t, std::unordered_map<c_size, c_size>>>> ReactionData::mapReactantsToMolecules(
	const std::vector<Molecule>& molecules) const
{
	std::vector<std::vector<std::pair<size_t, std::unordered_map<c_size, c_size>>>> allowedMatches;

	for (size_t i = 0; i < reactants.size(); ++i)
	{
		allowedMatches.emplace_back();
		for (size_t j = 0; j < molecules.size(); ++j)
		{
			const auto match = reactants[i].matchWith(molecules[j].getStructure());
			if (match.size())
				allowedMatches.back().emplace_back(std::make_pair(j, match));
		}
		if (allowedMatches.back().empty())
			return std::vector<std::vector<std::pair<size_t, std::unordered_map<c_size, c_size>>>>();

		// skip repeated reactants
		while (i + 1 < reactants.size() && reactants[i].getId() == reactants[i + 1].getId())
			++i;
	}

	return allowedMatches;
}

std::vector<std::unordered_map<c_size, c_size>> ReactionData::generateConcreteMatches(
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
			Utils::extractValues(matches[p.first.first])
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

const std::vector<Reactable>& ReactionData::getReactants() const
{
	return reactants;
}

const std::vector<Reactable>& ReactionData::getProducts() const
{
	return products;
}

const std::vector<Catalyst>& ReactionData::getCatalysts() const
{
	return catalysts;
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
