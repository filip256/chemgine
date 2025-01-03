#include "Molecule.hpp"
#include "DataStore.hpp"
#include "Formulas.hpp"

Molecule::Molecule(const MoleculeData& data) noexcept :
	data(data)
{}

Molecule::Molecule(MolecularStructure&& structure) noexcept :
	data(Accessor<>::getDataStore().molecules.findOrAddConcrete(std::move(structure))),
	molarMass(data.getStructure().getMolarMass())
{}

Molecule::Molecule(const std::string& smiles) noexcept :
	Molecule(MolecularStructure(smiles))
{}

MoleculeId Molecule::getId() const
{
	return data.id;
}

Amount<Unit::GRAM_PER_MOLE> Molecule::getMolarMass() const
{
	return molarMass;
}

const MoleculeData& Molecule::getData() const
{
	return data;
}

const MolecularStructure& Molecule::getStructure() const
{
	return data.getStructure();
}

Polarity Molecule::getPolarity() const
{
	return data.polarity;
}

Color Molecule::getColor() const
{
	return data.color;
}

Quantity<AbsCelsius> Molecule::getMeltingPointAt(const Quantity<Torr> pressure) const
{
	return data.meltingPointEstimator->get(pressure);
}

Quantity<AbsCelsius> Molecule::getBoilingPointAt(const Quantity<Torr> pressure) const
{
	return data.boilingPointEstimator->get(pressure);
}

AggregationType Molecule::getAggregationAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	return temperature > getBoilingPointAt(pressure.asStd() * _Torr).value() ? AggregationType::GAS :
		temperature > getMeltingPointAt(pressure.asStd() * _Torr).value() ? AggregationType::LIQUID :
		AggregationType::SOLID;
}

Amount<Unit::GRAM_PER_MILLILITER> Molecule::getDensityAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure,
	const AggregationType aggregation
) const
{
	return
		aggregation == AggregationType::GAS ? Formulas::idealGasLaw(temperature, pressure, molarMass) :
		aggregation == AggregationType::LIQUID ? Amount<Unit::GRAM_PER_MILLILITER>(data.liquidDensityEstimator->get(temperature.asStd() * _Celsius).value()) :
		data.solidDensityEstimator->get(temperature.asStd() * _Celsius).value();
}

Quantity<GramPerMilliLiter> Molecule::getDensityAt(
	const Quantity<Celsius> temperature,
	const Quantity<Torr> pressure
) const
{
	return getDensityAt(temperature.value(), pressure.value(), getAggregationAt(temperature.value(), pressure.value())).asStd() * _GramPerMilliLiter;
}

Amount<Unit::JOULE_PER_MOLE_CELSIUS> Molecule::getHeatCapacityAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure,
	const AggregationType aggregation
) const
{
	return
		aggregation == AggregationType::GAS ? Formulas::isobaricHeatCapacity(data.getStructure().getDegreesOfFreedom()) :
		aggregation == AggregationType::LIQUID ? Amount<Unit::JOULE_PER_MOLE_CELSIUS>(data.liquidHeatCapacityEstimator->get(pressure.asStd() * _Torr).value()) :
		data.solidHeatCapacityEstimator->get(pressure.asStd() * _Torr).value();
}

Amount<Unit::JOULE_PER_MOLE_CELSIUS> Molecule::getHeatCapacityAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	return getHeatCapacityAt(temperature, pressure, getAggregationAt(temperature, pressure));
}

Amount<Unit::JOULE_PER_MOLE> Molecule::getFusionHeatAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	return data.fusionLatentHeatEstimator->get(temperature.asStd() * _Celsius, -pressure.asStd() * _Torr).value();
}

Amount<Unit::JOULE_PER_MOLE> Molecule::getVaporizationHeatAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	return data.vaporizationLatentHeatEstimator->get(temperature.asStd() * _Celsius, pressure.asStd() * _Torr).value();
}

Amount<Unit::JOULE_PER_MOLE> Molecule::getSublimationHeatAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	return data.sublimationLatentHeatEstimator->get(temperature.asStd() * _Celsius, pressure.asStd() * _Torr).value();
}

Amount<Unit::JOULE_PER_MOLE> Molecule::getLiquefactionHeatAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	return -getFusionHeatAt(temperature, pressure);
}

Amount<Unit::JOULE_PER_MOLE> Molecule::getCondensationHeatAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	return -getVaporizationHeatAt(temperature, -pressure);
}

Amount<Unit::JOULE_PER_MOLE> Molecule::getDepositionHeatAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	return -getSublimationHeatAt(temperature, -pressure);
}

Amount<Unit::MOLE_RATIO> Molecule::getSolubilityAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure,
	const Polarity& solventPolarity
) const
{
	if (this->getAggregationAt(temperature, pressure) == AggregationType::GAS)
		return ((pressure.asStd() * _Torr) / data.henrysConstantEstimator->get(temperature.asStd() * _Celsius)).value();

	// Direct approach
	//
	const auto baseSolubility =
		(data.polarity.hydrophilicity * solventPolarity.hydrophilicity + data.polarity.lipophilicity * solventPolarity.lipophilicity) /
		(solventPolarity.hydrophilicity + solventPolarity.lipophilicity);

	// Logarithmic approach
	// 
	//const auto logP = solventPolarity.getPartitionCoefficient().asStd();
	//const auto phi = 2.5;
	//const auto baseSolubility =
	//    logP <= -phi ? solute.polarity.hydrophilicity :
	//	  logP >= phi ? solute.polarity.lipophilicity :
	//	  (solute.polarity.hydrophilicity * (phi - logP) + solute.polarity.lipophilicity * (phi + logP)) / (2.0 * phi);

	const auto scale = data.relativeSolubilityEstimator->get(temperature.asStd() * _Celsius);
	return baseSolubility * scale.value();
}

bool Molecule::operator==(const Molecule& other) const
{
	return this->data.id == other.data.id;
}

bool Molecule::operator!=(const Molecule& other) const
{
	return this->data.id != other.data.id;
}
