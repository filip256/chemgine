#pragma once

#include "Quantity.hpp"
#include "DynamicQuantity.hpp"
#include "Location.hpp"
#include "Log.hpp"

template<UnitType OutU, UnitType... InUs>
class DataPoint
{
public:
    const Quantity<OutU> output;
    const std::tuple<Quantity<InUs>...> inputs;

	DataPoint(
		const Quantity<OutU> output,
		const std::tuple<Quantity<InUs>...>& inputs
	) noexcept;

	template<typename = std::enable_if_t<(sizeof...(InUs) > 1)>>
	DataPoint(
		const Quantity<OutU> output,
		const Quantity<InUs>... inputs
	) noexcept;

	DataPoint(const DataPoint&) = default;

	bool operator==(const DataPoint& other) const;
	bool operator<(const DataPoint& other) const;
	bool operator>(const DataPoint& other) const;
};

template<UnitType OutU, UnitType... InUs>
DataPoint<OutU, InUs...>::DataPoint(
	const Quantity<OutU> output,
	const std::tuple<Quantity<InUs>...>& inputs
) noexcept :
	output(output),
	inputs(inputs)
{}

template<UnitType OutU, UnitType... InUs>
template<typename>
DataPoint<OutU, InUs...>::DataPoint(
	const Quantity<OutU> output,
	const Quantity<InUs>... inputs
) noexcept :
	DataPoint<OutU, InUs...>(output, std::make_tuple(inputs...))
{}

template<UnitType OutU, UnitType... InUs>
bool DataPoint<OutU, InUs...>::operator==(const DataPoint& other) const
{
	return this->output == other.output && this->inputs == other.inputs;
}

template<UnitType OutU, UnitType... InUs>
bool DataPoint<OutU, InUs...>::operator<(const DataPoint& other) const
{
	return this->inputs < other.inputs;
}

template<UnitType OutU, UnitType... InUs>
bool DataPoint<OutU, InUs...>::operator>(const DataPoint& other) const
{
	return this->inputs > other.inputs;
}


template<UnitType OutU, UnitType... InUs>
class Def::Parser<DataPoint<OutU, InUs...>>
{
private:
	template<std::size_t... Is>
	static std::optional<std::tuple<Quantity<InUs>...>> convertInputs(
		const std::vector<DynamicQuantity>& baseInputs,
		const Def::Location& location,
		std::index_sequence<Is...>)
	{
		std::tuple<Quantity<InUs>...> expectedInputs;
		auto failed = static_cast<uint8_t>(-1);

		(
			[&baseInputs, &expectedInputs, &failed, &location]() {
				const auto expected = baseInputs[Is].to<InUs>();
				if (not expected)
				{
					failed = Is;
					return;
				}

				std::get<Is>(expectedInputs) = *expected;
			}(), ...
		);

		return failed == static_cast<uint8_t>(-1) ?
			std::optional(expectedInputs) :
			std::nullopt;
	}

public:
	static std::optional<DataPoint<OutU, InUs...>> parse(
		const std::string& str,
		const UnitId outputBaseUnit,
		const std::vector<UnitId>& inputBaseUnits,
		const Def::Location& location
	)
	{
		const Log<DataPoint<OutU, InUs...>> log;
		const auto inputCount = sizeof...(InUs);

		// parse values
		const auto pair = Def::parse<std::pair<std::string, DynamicQuantity>>(str, ':');
		if (not pair)
		{
			log.error("Malfomed data point: '{0}', at: {1}.", str, location.toString());
			return std::nullopt;
		}

		const auto rawInputs = Def::parse<std::vector<DynamicQuantity>>(pair->first, ',');
		if (not rawInputs)
		{
			log.error("Malfomed data point inputs list: '{0}', at: {1}.", pair->first, location.toString());
			return std::nullopt;
		}
		if (rawInputs->size() != inputCount)
		{
			log.error("The number of data point inputs ({0}) in: '{1}' does not match the expected number of inputs ({2}), at: {3}.",
				rawInputs->size(), pair->first, inputCount, location.toString());
			return std::nullopt;
		}

		// convert output
		const auto baseOutput = pair->second.to(outputBaseUnit);
		if (not baseOutput)
		{
			log.error("Failed to convert data point output from given unit: '{0}' to base unit: '{1}', at: {2}.",
				pair->second.unitSymbol(), DynamicQuantity::getUnitSymbol(outputBaseUnit), location.toString());
			return std::nullopt;
		}

		const auto expectedOutput = baseOutput->to<OutU>();
		if (not expectedOutput)
		{
			log.error("Failed to convert data point output from base unit: '{0}' to expected unit: '{1}', at: {2}.",
				baseOutput->unitSymbol(), Quantity<OutU>::getUnitSymbol(), location.toString());
			return std::nullopt;
		}

		// convert inputs
		std::vector<DynamicQuantity> baseInputs;
		baseInputs.reserve(inputCount);
		for (size_t i = 0; i < inputCount; i++)
		{
			const auto convert = (*rawInputs)[i].to(inputBaseUnits[i]);
			if (not convert)
			{
				log.error("Failed to convert data point input from given unit: '{0}' to base unit: '{1}', at: {2}.",
					(*rawInputs)[i].unitSymbol(), DynamicQuantity::getUnitSymbol(inputBaseUnits[i]), location.toString());
				return std::nullopt;
			}

			baseInputs.emplace_back(*convert);
		}

		// try given input order
		const auto expectedInputs = convertInputs(baseInputs, location, std::make_index_sequence<inputCount>{});
		if (expectedInputs)
			return DataPoint(*expectedOutput, *expectedInputs);

		// try to permute input order (must sort before calling next_permutation)
		std::sort(baseInputs.begin(), baseInputs.end(),
			[](const auto& l, const auto& r) { return l.getUnit() < r.getUnit(); });
		do
		{
			const auto expectedInputs = convertInputs(baseInputs, location, std::make_index_sequence<inputCount>{});
			if (expectedInputs)
				return DataPoint(*expectedOutput, *expectedInputs);

		} while (std::next_permutation(baseInputs.begin(), baseInputs.end(),
			[](const auto& l, const auto& r) { return l.getUnit() < r.getUnit(); }));

		std::string baseUnitNames = DynamicQuantity::getUnitSymbol(inputBaseUnits.front());
		for (size_t i = 1; i < inputBaseUnits.size(); ++i)
			baseUnitNames += ", " + DynamicQuantity::getUnitSymbol(inputBaseUnits[i]);

		std::string expectedUnitNames;
		((expectedUnitNames += Quantity<InUs>::getUnitSymbol() + ", "), ...);
		expectedUnitNames.pop_back();
		expectedUnitNames.pop_back();

		log.error("Failed to convert data point inputs from base units: '{0}' to expected units: '{1}', at: {2}.",
			baseUnitNames, expectedUnitNames, location.toString());
		return std::nullopt;
	}
};

template<UnitType OutU, UnitType... InUs>
class Def::Printer<DataPoint<OutU, InUs...>>
{
public:
	static std::string print(const DataPoint<OutU, InUs...>& object)
	{
		return Def::print(std::pair(object.inputs, object.output));
	}

	static std::string prettyPrint(const DataPoint<OutU, InUs...>& object)
	{
		return Def::prettyPrint(std::pair(object.inputs, object.output));
	}
};
