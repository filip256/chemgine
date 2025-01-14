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

Quantity<GramPerMole> Molecule::getMolarMass() const
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
	const Quantity<AbsCelsius> temperature,
	const Quantity<Torr> pressure
) const
{
	return temperature > getBoilingPointAt(pressure) ? AggregationType::GAS :
		temperature > getMeltingPointAt(pressure) ? AggregationType::LIQUID :
		AggregationType::SOLID;
}

Quantity<GramPerMilliLiter> Molecule::getDensityAt(
	const Quantity<AbsCelsius> temperature,
	const Quantity<Torr> pressure,
	const AggregationType aggregation
) const
{
	return
		aggregation == AggregationType::GAS ? Formulas::idealGasLaw(temperature, pressure, molarMass) :
		aggregation == AggregationType::LIQUID ? data.liquidDensityEstimator->get(temperature) :
		data.solidDensityEstimator->get(temperature).value();
}

Quantity<GramPerMilliLiter> Molecule::getDensityAt(
	const Quantity<AbsCelsius> temperature,
	const Quantity<Torr> pressure
) const
{
	return getDensityAt(temperature.value(), pressure.value(), getAggregationAt(temperature.value(), pressure.value())).asStd() * _GramPerMilliLiter;
}

Quantity<JoulePerMoleCelsius> Molecule::getHeatCapacityAt(
	const Quantity<AbsCelsius> temperature,
	const Quantity<Torr> pressure,
	const AggregationType aggregation
) const
{
	return
		aggregation == AggregationType::GAS ? Formulas::isobaricHeatCapacity(data.getStructure().getDegreesOfFreedom()) :
		aggregation == AggregationType::LIQUID ? Quantity<JoulePerMoleCelsius>(data.liquidHeatCapacityEstimator->get(pressure.asStd() * _Torr).value()) :
		data.solidHeatCapacityEstimator->get(pressure.asStd() * _Torr).value();
}

Quantity<JoulePerMoleCelsius> Molecule::getHeatCapacityAt(
	const Quantity<AbsCelsius> temperature,
	const Quantity<Torr> pressure
) const
{
	return getHeatCapacityAt(temperature, pressure, getAggregationAt(temperature, pressure));
}

Quantity<JoulePerMole> Molecule::getFusionHeatAt(
	const Quantity<AbsCelsius> temperature,
	const Quantity<Torr> pressure
) const
{
	return data.fusionLatentHeatEstimator->get(temperature.asStd() * _Celsius, -pressure.asStd() * _Torr).value();
}

Quantity<JoulePerMole> Molecule::getVaporizationHeatAt(
	const Quantity<AbsCelsius> temperature,
	const Quantity<Torr> pressure
) const
{
	return data.vaporizationLatentHeatEstimator->get(temperature.asStd() * _Celsius, pressure.asStd() * _Torr).value();
}

Quantity<JoulePerMole> Molecule::getSublimationHeatAt(
	const Quantity<AbsCelsius> temperature,
	const Quantity<Torr> pressure
) const
{
	return data.sublimationLatentHeatEstimator->get(temperature.asStd() * _Celsius, pressure.asStd() * _Torr).value();
}

Quantity<JoulePerMole> Molecule::getLiquefactionHeatAt(
	const Quantity<AbsCelsius> temperature,
	const Quantity<Torr> pressure
) const
{
	return -getFusionHeatAt(temperature, pressure);
}

Quantity<JoulePerMole> Molecule::getCondensationHeatAt(
	const Quantity<AbsCelsius> temperature,
	const Quantity<Torr> pressure
) const
{
	return -getVaporizationHeatAt(temperature, -pressure);
}

Quantity<JoulePerMole> Molecule::getDepositionHeatAt(
	const Quantity<AbsCelsius> temperature,
	const Quantity<Torr> pressure
) const
{
	return -getSublimationHeatAt(temperature, -pressure);
}

Quantity<MoleRatio> Molecule::getSolubilityAt(
	const Quantity<AbsCelsius> temperature,
	const Quantity<Torr> pressure,
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
