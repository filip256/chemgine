#include "ReactionData.hpp"
#include "SystemMatrix.hpp"
#include "Maths.hpp"

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
				system.back()[i] = static_cast<double>(c.second);
			}
			else
				system[sysmap[c.first]][i] = static_cast<double>(c.second);
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
			system.back().back() = static_cast<double>(c.second);
		}
		else
			system[sysmap[c.first]].back() = static_cast<double>(c.second);
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
				system.back()[reactants.size() + i - 1] = -1 * static_cast<double>(c.second);
			}
			else
				system[sysmap[c.first]][reactants.size() + i - 1] = -1 * static_cast<double>(c.second);
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