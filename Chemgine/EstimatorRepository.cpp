#include "EstimatorRepository.hpp"
#include "DataHelpers.hpp"
#include "Keywords.hpp"
#include "Log.hpp"

#include <cmath>

constexpr EstimatorId toId(const BuiltinEstimator tag)
{
	return static_cast<EstimatorId>(tag);
}

EstimatorRepository::EstimatorRepository() noexcept
{
	// TODO: remove this ctor and define estimators in .cdef file

	table.emplace(std::make_pair(toId(BuiltinEstimator::TEMP_TO_REL_RSPEED),
		new FunctionalEstimator(toId(BuiltinEstimator::TEMP_TO_REL_RSPEED),
			+[](double deltaTemperature) { return std::pow(2, deltaTemperature / 10.0); })
	));

	table.emplace(std::make_pair(toId(BuiltinEstimator::MCONC_TO_REL_RSPEED),
		new FunctionalEstimator(toId(BuiltinEstimator::MCONC_TO_REL_RSPEED),
			+[](double molarConcentration) { return std::pow(molarConcentration, 0.25); })
	));

	table.emplace(std::make_pair(toId(BuiltinEstimator::TEMP_TO_DENSITY),
		new SplineEstimator(toId(BuiltinEstimator::TEMP_TO_DENSITY),
			Spline<float> {{-30.0f, 0.984f}, { -20.0f, 0.993f }, { 4.0f, 1.0f }, { 30.0f, 0.996f } })
	));

	table.emplace(std::make_pair(toId(BuiltinEstimator::TORR_TO_REL_BP),
		new FunctionalEstimator(toId(BuiltinEstimator::TORR_TO_REL_BP),
			+[](double torr) { return (torr - 760) * 0.045; })
	));

	// Describes how latent heats change with temperature and pressure.
	//  - if temperature is higher than bp/mp less heat is required
	//  - depending on the type of lantent heat, pressure can either lower or increase it:
	//     - pass P for expansion heats
	//     - pass -P for compression heats
	table.emplace(std::make_pair(toId(BuiltinEstimator::TDIF_TORR_TO_REL_LH),
		new FunctionalEstimator(toId(BuiltinEstimator::TDIF_TORR_TO_REL_LH),
			+[](double tempDifC, double torr) { return (std::pow(1.005, -tempDifC / 2.0) +
				torr > 0 ?
				std::pow(1.001, (torr - 760) / 2.0) :
				-1 * std::pow(1.001, (-torr - 760) / 2.0))
			/ 2.0; })
	));

	table.emplace(std::make_pair(toId(BuiltinEstimator::TEMP_TO_REL_SOL),
		new FunctionalEstimator(toId(BuiltinEstimator::TEMP_TO_REL_SOL),
			+[](double temp) { return temp * 0.001 + 1; })
	));

	table.emplace(std::make_pair(toId(BuiltinEstimator::TEMP_TO_REL_INV_SOL),
		new FunctionalEstimator(toId(BuiltinEstimator::TEMP_TO_REL_INV_SOL),
			+[](double temp) { return temp * -0.001 + 1; })
	));
}

EstimatorRepository::~EstimatorRepository() noexcept
{
	for (const auto& a : table)
		delete a.second;
}

bool EstimatorRepository::addOOLDefinition(DefinitionObject&& definition)
{
	auto specifier = DataHelpers::parse<EstimatorSpecifier>(definition.getSpecifier());
	if (specifier.has_value() == false)
	{
		Log(this).error("Invalid spline units specifier: '{0}', as: {1}.", definition.getSpecifier(), definition.getLocationName());
		return false;
	}

	const auto rawValues = definition.pullProperty(Keywords::Splines::Values,
		DataHelpers::parse<std::vector<std::pair<DynamicAmount, DynamicAmount>>>);

	if (rawValues.has_value() == false)
	{
		Log(this).error("Incomplete spline definition at: {0}.", definition.getLocationName());
		return false;
	}

	std::vector<std::pair<float, float>> values;
	values.reserve(rawValues->size());

	for (size_t i = 0; i < rawValues->size(); ++i)
	{
		const auto& pair = (*rawValues)[i];
		const auto inAmount = pair.first.to(specifier->inUnit);
		if (inAmount.has_value() == false)
		{
			Log(this).error("Failed to convert spline value input unit: '{0}' to base input unit: '{1}', at: {2}",
				pair.first.getUnitSymbol(), DynamicAmount::getUnitSymbol(specifier->inUnit), definition.getLocationName());
			return false;
		}

		const auto outAmount = pair.second.to(specifier->outUnit);
		if (outAmount.has_value() == false)
		{
			Log(this).error("Failed to convert spline value output unit: '{0}' to base input unit: '{1}', at: {2}",
				pair.second.getUnitSymbol(), DynamicAmount::getUnitSymbol(specifier->outUnit), definition.getLocationName());
			return false;
		}

		values.emplace_back(inAmount->asStd(), outAmount->asStd());
	}

	const auto& ignored = definition.getRemainingProperties();
	for (const auto& [name, _] : ignored)
		Log(this).warn("Ignored unknown spline property: '{0}', at: {1}.", name, definition.getLocationName());

	const auto success = oolSplines.emplace(definition.getIdentifier(), OOLSplineEstimator(std::move(*specifier), std::move(values)));
	if (success.second == false)
	{
		Log(this).error("Clash between existing spline identifier: '{0}' and new definition at: {1}.", success.first->first, definition.getLocationName());
		return false;
	}

	return true;
}

const BaseEstimator& EstimatorRepository::add(const BaseEstimator* estimator)
{
	const auto it = std::find_if(table.cbegin(), table.cend(), [estimator](const auto& e) {
		return e.second->isEquivalent(*estimator);
	});

	if (it != table.end())
	{
		delete estimator;
		return *it->second;
	}

	table.emplace(std::make_pair(estimator->id, estimator));
	return *estimator;
}

const BaseEstimator& EstimatorRepository::at(const EstimatorId id) const
{
	return *table.at(id);
}

EstimatorId EstimatorRepository::getFreeId() const
{
	size_t id = 201;
	while (table.contains(id) && id != 0) ++id; // overflow protection
	return id;
}
