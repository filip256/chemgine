#include "EstimatorRepository.hpp"
#include "Parsers.hpp"
#include "Keywords.hpp"
#include "Log.hpp"

#include <cmath>

// TODO: remove comment

// Describes how latent heats change with temperature and pressure.
//  - if temperature is higher than bp/mp less heat is required
//  - depending on the type of lantent heat, pressure can either lower or increase it:
//     - pass P for expansion heats
//     - pass -P for compression heats
//table.emplace(std::make_pair(toId(BuiltinEstimator::TDIF_TORR_TO_REL_LH),
//	new FunctionalEstimator(toId(BuiltinEstimator::TDIF_TORR_TO_REL_LH),
//		+[](double tempDifC, double torr) { return (std::pow(1.005, -tempDifC / 2.0) +
//			torr > 0 ?
//			std::pow(1.001, (torr - 760) / 2.0) :
//			-1 * std::pow(1.001, (-torr - 760) / 2.0))
//		/ 2.0; })
//));

const EstimatorBase& EstimatorRepository::add(std::unique_ptr<const EstimatorBase>&& estimator)
{
	const auto it = std::find_if(table.cbegin(), table.cend(), [&estimator](const auto& e) {
		return e.second->isEquivalent(*estimator);
	});

	if (it != table.end())
		return *it->second;

	const auto inserted = table.emplace(estimator->id, std::move(estimator));
	return *inserted.first->second;
}

const EstimatorBase& EstimatorRepository::at(const EstimatorId id) const
{
	return *table.at(id);
}

EstimatorId EstimatorRepository::getFreeId() const
{
	size_t id = 201;
	while (table.contains(id) && id != 0) ++id; // overflow protection
	return id;
}
