#include "EstimatorDataTable.hpp"
#include "Logger.hpp"

#include <cmath>

EstimatorDataTable::~EstimatorDataTable() noexcept
{
	for (const auto& a : table)
		delete a.second;
}

void EstimatorDataTable::addPredefined()
{
	table.emplace(std::make_pair(static_cast<EstimatorIdType>(Estimators::TEMP_TO_REL_RSPEED),
		new FunctionalEstimator(static_cast<EstimatorIdType>(Estimators::TEMP_TO_REL_RSPEED),
			+[](double deltaTemperature) { return std::pow(2, deltaTemperature / 10.0); })
	));

	table.emplace(std::make_pair(static_cast<EstimatorIdType>(Estimators::MCONC_TO_REL_RSPEED),
		new FunctionalEstimator(static_cast<EstimatorIdType>(Estimators::MCONC_TO_REL_RSPEED),
			+[](double molarConcentration) { return std::pow(molarConcentration, 0.25); })
	));

	table.emplace(std::make_pair(static_cast<EstimatorIdType>(Estimators::TEMP_TO_DENSITY),
		new SplineEstimator(static_cast<EstimatorIdType>(Estimators::TEMP_TO_DENSITY),
			Spline<float> {{-30.0f, 0.984f}, { -20.0f, 0.993f }, { 4.0f, 1.0f }, { 30.0f, 0.996f } })
	));

	table.emplace(std::make_pair(static_cast<EstimatorIdType>(Estimators::TORR_TO_REL_BP),
		new FunctionalEstimator(static_cast<EstimatorIdType>(Estimators::TORR_TO_REL_BP),
			+[](double torr) { return (torr - 760) * 0.045; })
	));

	// Describes how latent heats change with temperature and pressure.
	//  - if temperature is higher than bp/mp less heat is required
	//  - depending on the type of lantent heat, pressure can either lower or increase it:
	//     - pass P for expansion heats
	//     - pass -P for compression heats
	table.emplace(std::make_pair(static_cast<EstimatorIdType>(Estimators::TDIF_TORR_TO_REL_LH),
		new FunctionalEstimator(static_cast<EstimatorIdType>(Estimators::TDIF_TORR_TO_REL_LH),
			+[](double tempDifC, double torr) { return (std::pow(1.005, -tempDifC / 2.0) +
				torr > 0 ? 
					std::pow(1.001, (torr - 760) / 2.0) :
					-1 * std::pow(1.001, (-torr - 760) / 2.0))
			/ 2.0; })
	));
}

bool EstimatorDataTable::loadFromFile(const std::string& path)
{
	addPredefined();

	Logger::log("Loaded " + std::to_string(table.size()) + " estimators.", LogType::GOOD);
	return true;
}

const BaseEstimator& EstimatorDataTable::add(const BaseEstimator* estimator)
{
	// TODO: Check if estimator already exists and return that instance instead
	table.emplace(std::make_pair(estimator->id, estimator));
	return *estimator;
}

const BaseEstimator& EstimatorDataTable::at(const EstimatorIdType id) const
{
	return *table.at(id);
}

EstimatorIdType EstimatorDataTable::getFreeId() const
{
	size_t id = 201;
	while (table.contains(id) && id != 0) ++id; // overflow protection
	return id;
}