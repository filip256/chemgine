#pragma once

#include "BaseEstimator.hpp"
#include "EstimatorSpecifier.hpp"
#include "DataPoint.hpp"
#include "DefinitionObject.hpp"
#include "Amount.hpp"

template<Unit OutU, Unit... InUs>
class TypedEstimator : public BaseEstimator
{
public:
	using BaseEstimator::BaseEstimator;

	virtual Amount<OutU> get(const Amount<InUs>... inputs) const = 0;
};


template<Unit OutU, Unit... InUs>
class Def::Parser<TypedEstimator<OutU, InUs...>*>
{
public:
	static std::optional<TypedEstimator<OutU, InUs...>*> parse(
		const DefinitionObject& definition)
	{
		const Log<Parser<TypedEstimator<OutU, InUs...>*>> log;

		auto specifier = Def::parse<EstimatorSpecifier>(definition.getSpecifier());
		if (not specifier.has_value())
		{
			log.error("Malfomed data units specifier: '{0}', at: {1}.", definition.getSpecifier(), definition.getLocationName());
			return std::nullopt;
		}

		const uint8_t expectedInputCount = sizeof...(InUs);
		if (specifier->inUnits.size() != expectedInputCount)
		{
			log.error("The number of input base units: {0} in: '{1}' does not match the expected number of inputs: {2}, at: {3}.",
				specifier->inUnits.size(), definition.getSpecifier(), expectedInputCount, definition.getLocationName());
			return std::nullopt;
		}

		const auto strValues = definition.getProperty(Keywords::Data::Values,
			Def::parse<std::vector<std::string>>);
		if (not strValues.has_value())
			return std::nullopt;

		std::vector<DataPoint<OutU, InUs...>> dataPoints;
		for (size_t i = 0; i < strValues->size(); ++i)
		{
			const auto point = Def::parse<DataPoint<OutU, InUs...>>(
				(*strValues)[i], specifier->outUnit, specifier->inUnits, definition.getLocation());
			if(not point.has_value())
				return std::nullopt;

			dataPoints.emplace_back(*point);
		}

		return nullptr;
	}
};
