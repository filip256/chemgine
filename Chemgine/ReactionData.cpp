#include "ReactionData.hpp"
#include "SystemMatrix.hpp"

ReactionData::ReactionData(
	const ReactionIdType id,
	const std::string& name,
	std::vector<std::pair<const Reactable*, uint8_t>>&& reactants,
	std::vector<std::pair<const Reactable*, uint8_t>>&& products
) noexcept :
	id(id),
	name(name),
	reactants(std::move(reactants)),
	products(std::move(products))
{}

ReactionData::~ReactionData() noexcept
{
	while (reactants.empty() == false)
	{
		delete reactants.back().first;
		reactants.pop_back();
	}
	while (products.empty() == false)
	{
		delete products.back().first;
		products.pop_back();
	}
}

bool ReactionData::balance()
{
	SystemMatrix<float> system;
	const size_t syslen = reactants.size() + products.size();
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
				system.back()[i] = c.second;
				system.back().back() = reactants[i].second;
			}
			else
			{
				const auto temp = sysmap[c.first];
				system[temp][i] = c.second;
				system[temp].back() = reactants[i].second;
			}
		}
	}

	for (size_t i = 0; i < products.size(); ++i)
	{
		const auto map = products[i].first->getStructure().getComponentCountMap();
		for (const auto& c : map)
		{
			if (sysmap.contains(c.first) == false)
			{
				sysmap.emplace(std::move(std::make_pair(c.first, sysmap.size())));
				system.addRow(syslen);
				system.back()[i] = c.second;
				system.back().back() = products[i].second;
			}
			else
			{
				const auto temp = sysmap[c.first];
				system[temp][i] = c.second;
				system[temp].back() = products[i].second;
			}
		}
	}

	const auto result = system.solve();
	if (result.empty())
		return false;

	for (size_t i = 0; i < reactants.size(); ++i)
		reactants[i].second = result[i];
	for (size_t i = 0; i < products.size(); ++i)
		products[i].second = result[reactants.size() + i];

	return true;
}