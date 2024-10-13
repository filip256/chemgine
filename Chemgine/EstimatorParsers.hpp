#pragma once

#include "Keywords.hpp"
#include "EstimatorFactory.hpp"
#include "EstimatorSpecifier.hpp"

template<Unit OutU, Unit... InUs>
class Def::Parser<UnitizedEstimator<OutU, InUs...>>
{
public:
	static std::optional<EstimatorRef<OutU, InUs...>> parse(
		const DefinitionObject& definition,
		EstimatorRepository& repository)
	{
		const Log<Parser<UnitizedEstimator<OutU, InUs...>>> log;

		if (definition.getType() != DefinitionType::DATA && definition.getType() != DefinitionType::AUTO)
		{
			log.error("Expected definition of type 'DATA' but got: '{0}', at: {1}.", static_cast<uint8_t>(definition.getType()), definition.getLocationName());
			return std::nullopt;
		}

		const auto specifier = Def::parse<EstimatorSpecifier>(definition.getSpecifier());
		if (not specifier)
		{
			log.error("Malfomed data units specifier: '{0}', at: {1}.", definition.getSpecifier(), definition.getLocationName());
			return std::nullopt;
		}

		const auto inputCount = sizeof...(InUs);
		if (specifier->inUnits.size() != inputCount)
		{
			log.error("The number of input base units ({0}) in: '{1}' does not match the expected number of inputs ({2}), at: {3}.",
				specifier->inUnits.size(), definition.getSpecifier(), inputCount, definition.getLocationName());
			return std::nullopt;
		}

		if (const auto constant = definition.getOptionalProperty(Keywords::Data::Constant, Def::parse<Amount<OutU>>))
		{
			return EstimatorFactory(repository).createConstant<OutU, InUs...>(*constant);
		}

		if (const auto strValues = definition.getOptionalProperty(Keywords::Data::Values, Def::parse<std::vector<std::string>>))
		{
			const auto mode = definition.getDefaultProperty(Keywords::Data::Mode, EstimationMode::LINEAR,
				Def::parse<EstimationMode>);
			const auto loss = definition.getDefaultProperty(Keywords::Data::CompressionLoss, 0.0,
				Def::parse<float_n>);

			std::vector<DataPoint<OutU, InUs...>> dataPoints;
			for (size_t i = 0; i < strValues->size(); ++i)
			{
				const auto point = Def::parse<DataPoint<OutU, InUs...>>(
					(*strValues)[i], specifier->outUnit, specifier->inUnits, definition.getLocation());
				if (not point)
					return std::nullopt;

				dataPoints.emplace_back(*point);
			}

			return EstimatorFactory(repository).createData(std::move(dataPoints), mode, loss);
		}

		if (const auto parameters = definition.getOptionalProperty(Keywords::Data::Parameters,
			Def::parse<std::vector<float_n>>))
		{
			const auto mode = definition.getProperty(Keywords::Data::Mode, Def::parse<EstimationMode>);
			if (not mode)
				return std::nullopt;

			if constexpr (inputCount == 1)
			{
				if (mode == EstimationMode::LINEAR)
				{
					if (parameters->size() != 2)
					{
						log.error("Invalid number of parameters ({0}) for 1st degree linear regression.",
							parameters->size());
						return std::nullopt;
					}

					return EstimatorFactory(repository).createLinearRegression<OutU, InUs...>(
						parameters->front(), parameters->back());
				}
			}
			else if constexpr (inputCount == 2)
			{
				if (mode == EstimationMode::LINEAR)
				{
					if (parameters->size() != 3)
					{
						log.error("Invalid number of parameters ({0}) for 2nd degree linear regression.",
							parameters->size());
						return std::nullopt;
					}

					return EstimatorFactory(repository).createLinearRegression<OutU, InUs...>(
						parameters->front(), (*parameters)[1], parameters->back());
				}
			}

			log.fatal("Unsupported regression estimator definition, at: {0}", definition.getLocationName());
		}

		if constexpr (inputCount == 1)
		{
			constexpr Unit InU = std::get<0>(std::make_tuple(InUs...));

			const auto base = definition.getOptionalDefinition(Keywords::Data::Base,
				Def::Parser<UnitizedEstimator<OutU, InU>>::parse, repository);
			if (base)
			{
				// anchor transform
				if (const auto anchorPointStr = definition.getOptionalProperty(Keywords::Data::AnchorPoint))
				{
					const auto anchorPoint = Def::parse<DataPoint<OutU, InU>>(*anchorPointStr, OutU, std::vector<Unit>{InU}, definition.getLocation());
					if (not anchorPoint)
						return std::nullopt;

					const auto hShift = definition.getDefaultProperty(Keywords::Data::HorizontalShift, 0.0,
						Def::parse<float_n>);
					return EstimatorFactory(repository).createAffine(*base, *anchorPoint, hShift);
				}

				// rebase transform
				if (const auto rebasePointStr = definition.getOptionalProperty(Keywords::Data::RebasePoint))
				{
					const auto rebasePoint = Def::parse<DataPoint<OutU, InU>>(*rebasePointStr, OutU, std::vector<Unit>{InU}, definition.getLocation());
					if (not rebasePoint)
						return std::nullopt;

					return EstimatorFactory(repository).createAffine(*base, *rebasePoint);
				}

				// manual transform
				const auto vShift = definition.getDefaultProperty(Keywords::Data::VerticalShift, 0.0, Def::parse<float_n>);
				const auto hShift = definition.getDefaultProperty(Keywords::Data::HorizontalShift, 0.0, Def::parse<float_n>);
				const auto scale = definition.getDefaultProperty(Keywords::Data::Scale, 1.0, Def::parse<float_n>);

				return EstimatorFactory(repository).createAffine(*base, vShift, hShift, scale);
			}
		}

		log.fatal("Unsupported estimator definition, at: {0}", definition.getLocationName());
		return std::nullopt;
	}
};
