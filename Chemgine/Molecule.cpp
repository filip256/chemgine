#include "Molecule.hpp"
#include "DataStore.hpp"
#include "Amount.hpp"
#include "Formulas.hpp"

Molecule::Molecule(const MoleculeData& data) noexcept :
	data(data)
{}

Molecule::Molecule(MolecularStructure&& structure) noexcept :
	data(dataAccessor.getSafe().molecules.findOrAdd(std::move(structure))),
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

Amount<Unit::CELSIUS> Molecule::getMeltingPointAt(const Amount<Unit::TORR> pressure) const
{
	return data.meltingPointEstimator->get(pressure);
}

Amount<Unit::CELSIUS> Molecule::getBoilingPointAt(const Amount<Unit::TORR> pressure) const
{
	return data.boilingPointEstimator->get(pressure);
}

AggregationType Molecule::getAggregationAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	return temperature > getBoilingPointAt(pressure) ? AggregationType::GAS :
		temperature > getMeltingPointAt(pressure) ? AggregationType::LIQUID :
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
		aggregation == AggregationType::LIQUID ? data.liquidDensityEstimator->get(temperature) :
		data.solidDensityEstimator->get(temperature);
}

Amount<Unit::GRAM_PER_MILLILITER> Molecule::getDensityAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	return getDensityAt(temperature, pressure, getAggregationAt(temperature, pressure));
}

Amount<Unit::JOULE_PER_MOLE_CELSIUS> Molecule::getHeatCapacityAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure,
	const AggregationType aggregation
) const
{
	return
		aggregation == AggregationType::GAS ? Formulas::isobaricHeatCapacity(data.getStructure().getDegreesOfFreedom()) :
		aggregation == AggregationType::LIQUID ? data.liquidHeatCapacityEstimator->get(pressure) :
		data.solidHeatCapacityEstimator->get(pressure);
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
	return data.fusionLatentHeatEstimator->get(temperature, -pressure);
}

Amount<Unit::JOULE_PER_MOLE> Molecule::getVaporizationHeatAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	return data.vaporizationLatentHeatEstimator->get(temperature, pressure);
}

Amount<Unit::JOULE_PER_MOLE> Molecule::getSublimationHeatAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	return data.sublimationLatentHeatEstimator->get(temperature, pressure);
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
		return pressure.to<Unit::MOLE_RATIO>(data.henrysConstantEstimator->get(temperature));

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

	const auto scale = data.relativeSolubilityEstimator->get(temperature);
	return baseSolubility * scale.asStd();
}

std::string Molecule::getHRTag() const
{
	return data.getHRTag();
}

bool Molecule::operator==(const Molecule& other) const
{
	return this->data.id == other.data.id;
}

bool Molecule::operator!=(const Molecule& other) const
{
	return this->data.id != other.data.id;
}
