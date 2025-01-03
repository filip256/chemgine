#pragma once

#include "GenericMoleculeData.hpp"
#include "UnitizedEstimator.hpp"
#include "MoleculeType.hpp"
#include "Polarity.hpp"
#include "Color.hpp"

#include <string>

class MoleculeData : public GenericMoleculeData
{
public:
	const MoleculeType type;
	const std::string name;

	const Polarity polarity;

	const Color color;

	const EstimatorRef<AbsCelsius, Torr> meltingPointEstimator;
	const EstimatorRef<AbsCelsius, Torr> boilingPointEstimator;

	const EstimatorRef<GramPerMilliLiter, Celsius> solidDensityEstimator;
	const EstimatorRef<GramPerMilliLiter, Celsius> liquidDensityEstimator;

	const EstimatorRef<JoulePerMoleCelsius, Torr> solidHeatCapacityEstimator;
	const EstimatorRef<JoulePerMoleCelsius, Torr> liquidHeatCapacityEstimator;

	const EstimatorRef<JoulePerMole, Celsius, Torr> fusionLatentHeatEstimator;
	const EstimatorRef<JoulePerMole, Celsius, Torr> vaporizationLatentHeatEstimator;
	const EstimatorRef<JoulePerMole, Celsius, Torr> sublimationLatentHeatEstimator;

	const EstimatorRef<Dimless, Celsius> relativeSolubilityEstimator;
	const EstimatorRef<Torr, Celsius> henrysConstantEstimator;

	MoleculeData(
		const MoleculeId id,
		const std::string& name,
		MolecularStructure&& structure,
		const Amount<Unit::MOLE_RATIO> hydrophilicity,
		const Amount<Unit::MOLE_RATIO> lipophilicity,
		const Color color,
		EstimatorRef<AbsCelsius, Torr>&& meltingPointEstimator,
		EstimatorRef<AbsCelsius, Torr>&& boilingPointEstimator,
		EstimatorRef<GramPerMilliLiter, Celsius>&& solidDensityEstimator,
		EstimatorRef<GramPerMilliLiter, Celsius>&& liquidDensityEstimator,
		EstimatorRef<JoulePerMoleCelsius, Torr>&& solidHeatCapacityEstimator,
		EstimatorRef<JoulePerMoleCelsius, Torr>&& liquidHeatCapacityEstimator,
		EstimatorRef<JoulePerMole, Celsius, Torr>&& fusionLatentHeatEstimator,
		EstimatorRef<JoulePerMole, Celsius, Torr>&& vaporizationLatentHeatEstimator,
		EstimatorRef<JoulePerMole, Celsius, Torr>&& sublimationLatentHeatEstimator,
		EstimatorRef<Dimless, Celsius>&& relativeSolubilityEstimator,
		EstimatorRef<Torr, Celsius>&& henrysConstantEstimator
	) noexcept;

	MoleculeData(const MoleculeData&) = delete;
	MoleculeData(MoleculeData&&) = default;
	~MoleculeData() = default;

	const MolecularStructure& getStructure() const;

	void dumpDefinition(
		std::ostream& out,
		const bool prettify,
		std::unordered_set<EstimatorId>& alreadyPrinted
	) const;
	void print(std::ostream& out = std::cout) const;
};
