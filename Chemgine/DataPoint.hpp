#pragma once

#include "Amount.hpp"
#include "DynamicAmount.hpp"
#include "DefinitionLocation.hpp"
#include "Log.hpp"

template<Unit OutU, Unit... InUs>
class DataPoint
{
public:
    const Amount<OutU> output;
    const std::tuple<Amount<InUs>...> inputs;

	DataPoint(
		const Amount<OutU> output,
		const std::tuple<Amount<InUs>...>& inputs
	) noexcept;
	DataPoint(const DataPoint&) = default;
};

template<Unit OutU, Unit... InUs>
DataPoint<OutU, InUs...>::DataPoint(
	const Amount<OutU> output,
	const std::tuple<Amount<InUs>...>& inputs
) noexcept :
	output(output),
	inputs(inputs)
{}


template<Unit OutU, Unit... InUs>
class Def::Parser<DataPoint<OutU, InUs...>>
{
private:
	template<std::size_t... Is>
	static std::optional<std::tuple<Amount<InUs>...>> convertInputs(
		const std::vector<DynamicAmount>& baseInputs,
		const DefinitionLocation& location,
		std::index_sequence<Is...>)
	{
		std::tuple<Amount<InUs>...> expectedInputs;
		uint8_t failed = static_cast<uint8_t>(-1);

		(
			[&baseInputs, &expectedInputs, &failed, &location]() {
				const auto expected = baseInputs[Is].to<InUs>();
				if (not expected.has_value())
				{
					failed = Is;
					Log<DataPoint<OutU, InUs...>>().error("Failed to convert data point input from base unit: '{0}' to expected unit: '{1}', at: {2}.",
						baseInputs[Is].getUnitSymbol(), DynamicAmount::getUnitSymbol(InUs), location.toString());
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
		const Unit outputBaseUnit,
		const std::vector<Unit>& inputBaseUnits,
		const DefinitionLocation& location
	)
	{
		const Log<DataPoint<OutU, InUs...>> log;
		const uint8_t inputCount = sizeof...(InUs);

		// parse values
		const auto pair = Def::parse<std::pair<std::string, DynamicAmount>>(str, ':');
		if (not pair.has_value())
		{
			log.error("Malfomed data point: '{0}', at: {1}.", str, location.toString());
			return std::nullopt;
		}

		const auto rawInputs = Def::parse<std::vector<DynamicAmount>>(pair->first, ',');
		if (not rawInputs.has_value())
		{
			log.error("Malfomed data point inputs list: '{0}', at: {1}.", pair->first, location.toString());
			return std::nullopt;
		}
		if (rawInputs->size() != inputCount)
		{
			log.error("The number of data point inputs: {0} does not match the expected number of inputs: {1}, at: {2}.",
				rawInputs->size(), inputCount, location.toString());
			return std::nullopt;
		}

		// convert from element units to base units
		const auto baseOutput = pair->second.to(outputBaseUnit);
		if (not baseOutput.has_value())
		{
			log.error("Failed to convert data point output from given unit: '{0}' to base unit: '{1}', at: {2}.",
				pair->second.getUnitSymbol(), DynamicAmount::getUnitSymbol(outputBaseUnit), location.toString());
			return std::nullopt;
		}

		std::vector<DynamicAmount> baseInputs;
		baseInputs.reserve(inputCount);
		for (size_t i = 0; i < inputCount; i++)
		{
			const auto convert = (*rawInputs)[i].to(inputBaseUnits[i]);
			if (not convert.has_value())
			{
				log.error("Failed to convert data point input from given unit: '{0}' to base unit: '{1}', at: {2}.",
					(*rawInputs)[i].getUnitSymbol(), DynamicAmount::getUnitSymbol(inputBaseUnits[i]), location.toString());
				return std::nullopt;
			}

			baseInputs.emplace_back(*convert);
		}

		// convert from base units to expected static units
		const auto expectedOutput = baseOutput->to<OutU>();
		if (not expectedOutput.has_value())
		{
			log.error("Failed to convert data point output from base unit: '{0}' to expected unit: '{1}', at: {2}.",
				baseOutput->getUnitSymbol(), DynamicAmount::getUnitSymbol(OutU), location.toString());
			return std::nullopt;
		}

		const auto expectedInputs = convertInputs(baseInputs, location, std::make_index_sequence<inputCount>{});
		if (not expectedInputs.has_value())
			return std::nullopt;

		return DataPoint(*expectedOutput, *expectedInputs);
	}
};
