#include "ReactionData.hpp"
#include "SystemMatrix.hpp"
#include "Maths.hpp"

ReactionData::ReactionData(
	const ReactionIdType id,
	const std::string& name,
	const std::vector<std::pair<const Reactable*, uint8_t>>& reactants,
	const std::vector<std::pair<const Reactable*, uint8_t>>& products
) noexcept :
	id(id),
	name(name),
	reactants(flatten(reactants)),
	products(flatten(products))
{}

ReactionData::~ReactionData() noexcept
{
	while (reactants.size())
	{
		delete reactants.back();

		const auto last = reactants.back();
		do 
		{ 
			reactants.pop_back();
		} 
		while (reactants.size() && reactants.back() == last);
	}
	while (products.size())
	{
		delete products.back();
		
		const auto last = products.back();
		do 
		{ 
			products.pop_back(); 
		} 
		while (products.size() && products.back() == last);
	}
}

bool ReactionData::balance(
	std::vector<std::pair<const Reactable*, uint8_t>>& reactants,
	std::vector<std::pair<const Reactable*, uint8_t>>& products)
{
	if (reactants.size() == 0 || products.size() == 0)
		return false;

	SystemMatrix<float> system;
	const size_t syslen = reactants.size() + products.size() - 1;
	std::unordered_map<ComponentIdType, size_t> sysmap;

	for (size_t i = 0; i < reactants.size(); ++i)
	{
		const auto map = reactants[i].first->getStructure().getComponentCountMap();
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
	const auto map = products[0].first->getStructure().getComponentCountMap();
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
		const auto map = products[i].first->getStructure().getComponentCountMap();
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

	componentMapping.reserve((reactants.size() + products.size()) * 4); // why 4? most molecules have at least 4 atoms
	std::vector<std::unordered_set<c_size>> reactantIgnore(reactants.size()), productIgnore(products.size());

	for (size_t i = 0; i < reactants.size(); ++i)
	{
		std::pair<std::unordered_map<c_size, c_size>, uint8_t> maxMap;
		size_t maxIdxJ = 0;
		for (size_t j = 0; j < products.size(); ++j)
		{
			const auto map = reactants[i]->getStructure().maximalMapTo(products[j]->getStructure(), reactantIgnore[i], productIgnore[j]);
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
			componentMapping.emplace(std::make_pair(std::make_pair(i, p.first), std::make_pair(maxIdxJ, p.second)));
		}

		if (reactants[i]->getStructure().componentCount() != reactantIgnore[i].size())
			--i;
	}

	// check if mapping is complete
	for (size_t i = 0; i < reactants.size(); ++i)
	{
		if (reactants[i]->getStructure().componentCount() != reactantIgnore[i].size())
		{
			return false;
		}
	}
	for (size_t i = 0; i < products.size(); ++i)
	{
		if (products[i]->getStructure().componentCount() != productIgnore[i].size())
		{
			return false;
		}
	}

	return true;

}

std::vector<const Reactable*> ReactionData::flatten(
	const std::vector<std::pair<const Reactable*, uint8_t>>& list)
{
	std::vector<const Reactable*> result;
	result.reserve(list.size());

	for (size_t i = 0; i < list.size(); ++i)
		for(uint8_t j = 0; j < list[i].second; ++j)
			result.emplace_back(list[i].first);

	return result;
}