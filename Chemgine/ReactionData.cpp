#include "ReactionData.hpp"
#include "SystemMatrix.hpp"
#include "Maths.hpp"
#include "PairHash.hpp"
#include "Logger.hpp"
#include "Utils.hpp"

ReactionData::ReactionData(
	const ReactionIdType id,
	const std::string& name,
	const std::vector<std::pair<Reactable, uint8_t>>& reactants,
	const std::vector<std::pair<Reactable, uint8_t>>& products,
	const Amount<Unit::MOLE_PER_SECOND> baseSpeed,
	const Amount<Unit::CELSIUS> baseTemperature
) noexcept :
	id(id),
	baseSpeed(baseSpeed),
	baseTemperature(baseTemperature),
	name(name),
	reactants(flatten(reactants)),
	products(flatten(products))
{}

bool ReactionData::balance(
	std::vector<std::pair<Reactable, uint8_t>>& reactants,
	std::vector<std::pair<Reactable, uint8_t>>& products)
{
	if (reactants.size() == 0 || products.size() == 0)
		return false;

	SystemMatrix<float> system;
	const size_t syslen = reactants.size() + products.size() - 1;
	std::unordered_map<ComponentIdType, size_t> sysmap;

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
		reactants[i].second = result[i] * intCoef;
	products[0].second = intCoef;
	for (size_t i = 1; i < products.size(); ++i)
		products[i].second = result[reactants.size() + i - 1] *intCoef;

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

bool ReactionData::hasAsReactant(const Molecule& molecule) const
{
	for (size_t i = 0; i < reactants.size(); ++i)
	{
		if (molecule.getStructure().mapTo(reactants[i].getStructure(), true).size() != 0)
			return true;
	}
	return false;
}

void ReactionData::enumerateReactantPairs(
	const std::vector<Molecule>& molecules,
	const std::unordered_set<std::pair<size_t, size_t>, PairHash>& allowedPairs,
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

std::vector<Molecule> ReactionData::generateConcreteProducts(const std::vector<Molecule>& molecules) const
{
	auto x = &BaseComponent::instanceCount;
	if (reactants.size() != molecules.size())
		return std::vector<Molecule>();

	// find the molecule match for each reactant
	std::vector<std::unordered_map<c_size, c_size>> matches;
	matches.reserve(reactants.size());
	for (size_t i = 0; i < reactants.size(); ++i)
	{
		matches.emplace_back(Utils::reverseMap(reactants[i].matchWith(molecules[i].getStructure())));
		if (matches.back().empty() && reactants[i].getStructure().isVirtualHydrogen() == false)
			return std::vector<Molecule>();
	}

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
			molecules[p.first.first].getStructure(),
			matches[p.first.first].at(p.first.second),
			tempMap,
			false,
			Utils::extractValues(matches[p.first.first])
		);
	}

	// normalize
	std::vector<Molecule> result;
	result.reserve(concreteProducts.size());
	for (size_t i = 0; i < concreteProducts.size(); ++i)
	{
		concreteProducts[i].normalize();
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
