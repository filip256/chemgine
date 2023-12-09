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
	table.emplace(std::make_pair(101, 
		new FunctionalApproximator(101, "dtemp -> rspeed_multiplier", +[](double deltaTemperature) { return std::pow(2, deltaTemperature / 10.0); })
		));

	table.emplace(std::make_pair(102,
		new FunctionalApproximator(102, "mconc -> rspeed_multiplier", +[](double molarConcentration) { return std::pow(2, molarConcentration / 10.0); })
	));

	table.emplace(std::make_pair(103,
		new SplineApproximator(103, "temp -> density", Spline<float> {{-30.0f, 0.984f}, { -20.0f, 0.993f }, { 4.0f, 1.0f }, {30.0f, 0.996f} })
	));

	Logger::log("Loaded " + std::to_string(table.size()) + " approximators.", LogType::GOOD);
	return true;
}

const BaseApproximator& ApproximatorDataTable::at(const ApproximatorIdType id) const
{
	return *table.at(id);
}