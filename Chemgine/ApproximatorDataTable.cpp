#include "ApproximatorDataTable.hpp"
#include "Logger.hpp"

#include <cmath>

ApproximatorDataTable::~ApproximatorDataTable() noexcept
{
	for (const auto& a : table)
		delete a.second;
}

void ApproximatorDataTable::addPredefined()
{
	table.emplace(std::make_pair(static_cast<ApproximatorIdType>(Approximators::TEMP_TO_REL_RSPEED),
		new FunctionalApproximator(static_cast<ApproximatorIdType>(Approximators::TEMP_TO_REL_RSPEED),
			+[](double deltaTemperature) { return std::pow(2, deltaTemperature / 10.0); })
	));

	table.emplace(std::make_pair(static_cast<ApproximatorIdType>(Approximators::MCONC_TO_REL_RSPEED),
		new FunctionalApproximator(static_cast<ApproximatorIdType>(Approximators::MCONC_TO_REL_RSPEED),
			+[](double molarConcentration) { return std::pow(molarConcentration, 0.25); })
	));

	table.emplace(std::make_pair(static_cast<ApproximatorIdType>(Approximators::TEMP_TO_DENSITY),
		new SplineApproximator(static_cast<ApproximatorIdType>(Approximators::TEMP_TO_DENSITY),
			Spline<float> {{-30.0f, 0.984f}, { -20.0f, 0.993f }, { 4.0f, 1.0f }, { 30.0f, 0.996f } })
	));

	table.emplace(std::make_pair(static_cast<ApproximatorIdType>(Approximators::TORR_TO_REL_BP),
		new FunctionalApproximator(static_cast<ApproximatorIdType>(Approximators::TORR_TO_REL_BP),
			+[](double torr) { return (torr - 760) * 0.045; })
	));

	table.emplace(std::make_pair(static_cast<ApproximatorIdType>(Approximators::TEMP_DIF_TO_REL_LH),
		new FunctionalApproximator(static_cast<ApproximatorIdType>(Approximators::TEMP_DIF_TO_REL_LH),
			+[](double tempDif) { return std::pow(1 - tempDif / 50.0f, 0.63); })    // tempDif = actTemp - bp
	));
}

bool ApproximatorDataTable::loadFromFile(const std::string& path)
{
	addPredefined();

	Logger::log("Loaded " + std::to_string(table.size()) + " approximators.", LogType::GOOD);
	return true;
}

const BaseApproximator& ApproximatorDataTable::add(const BaseApproximator* approximator)
{
	// TODO: Check if appx already exists and return that instance instead
	table.emplace(std::make_pair(approximator->id, approximator));
	return *approximator;
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