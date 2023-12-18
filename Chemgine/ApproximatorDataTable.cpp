#include "ApproximatorDataTable.hpp"
#include "FunctionalApproximator.hpp"
#include "SplineApproximator.hpp"
#include "Logger.hpp"

#include <cmath>

ApproximatorDataTable::~ApproximatorDataTable() noexcept
{
	for (const auto& a : table)
		delete a.second;
}

bool ApproximatorDataTable::loadFromFile(const std::string& path)
{
	table.emplace(std::make_pair(static_cast<ApproximatorIdType>(Approximators::TEMP_TO_REL_RSPEED),
		new FunctionalApproximator(static_cast<ApproximatorIdType>(Approximators::TEMP_TO_REL_RSPEED),
			"dtemp -> relative rspeed", +[](double deltaTemperature) { return std::pow(2, deltaTemperature / 10.0); })
		));

	table.emplace(std::make_pair(static_cast<ApproximatorIdType>(Approximators::MCONC_TO_REL_RSPEED),
		new FunctionalApproximator(static_cast<ApproximatorIdType>(Approximators::MCONC_TO_REL_RSPEED), 
			"mconc -> relative rspeed", +[](double molarConcentration) { return std::pow(molarConcentration, 0.25); })
	));

	table.emplace(std::make_pair(static_cast<ApproximatorIdType>(Approximators::TEMP_TO_DENSITY),
		new SplineApproximator(static_cast<ApproximatorIdType>(Approximators::TEMP_TO_DENSITY), 
			"temp -> density", Spline<float> {{-30.0f, 0.984f}, { -20.0f, 0.993f }, { 4.0f, 1.0f }, {30.0f, 0.996f} })
	));

	table.emplace(std::make_pair(static_cast<ApproximatorIdType>(Approximators::TORR_TO_REL_BP),
		new FunctionalApproximator(static_cast<ApproximatorIdType>(Approximators::TORR_TO_REL_BP), 
			"pressure(torr) -> relative bp", +[](double torr) { return (torr - 760) * 0.045; })
	));

	Logger::log("Loaded " + std::to_string(table.size()) + " approximators.", LogType::GOOD);
	return true;
}

const BaseApproximator& ApproximatorDataTable::add(const BaseApproximator& approximator)
{
	BaseApproximator* appx = approximator.clone();
	appx->id = getFreeId();
	table.emplace(std::make_pair(appx->id, appx));
	return *appx;
}

const BaseApproximator& ApproximatorDataTable::at(const ApproximatorIdType id) const
{
	return *table.at(id);
}

ApproximatorIdType ApproximatorDataTable::getFreeId() const
{
	size_t id = 201;
	while (table.contains(id) && id != 0) ++id; // overflow protection
	return id;
}