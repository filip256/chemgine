#include "ApproximatorDataTable.hpp"
#include "Logger.hpp"

#include <cmath>

bool ApproximatorDataTable::loadFromFile(const std::string& path)
{
	table.emplace(std::make_pair(101, 
		FunctionalApproximator(101, "reaction speed approximator", +[](double deltaTemperature) { return std::pow(2, deltaTemperature / 10.0); })
		));

	Logger::log("Loaded " + std::to_string(table.size()) + " approximators.", LogType::GOOD);
	return true;
}


const FunctionalApproximator& ApproximatorDataTable::at(const ApproximatorIdType id) const
{
	return table.at(id);
}