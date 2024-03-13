#include "Molecule.hpp"
#include "DataStore.hpp"
#include "Amount.hpp"
#include "Formulas.hpp"

DataStoreAccessor Molecule::dataAccessor = DataStoreAccessor();

Molecule::Molecule(const MoleculeId id) noexcept :
	id(id),
	molarMass(dataAccessor.getSafe().molecules.at(id).getStructure().getMolarMass())
{}

Molecule::Molecule(MolecularStructure&& structure) noexcept :
	id(dataAccessor.getSafe().molecules.findOrAdd(std::move(structure)))
{
	molarMass = data().getStructure().getMolarMass();
}

Molecule::Molecule(const std::string& smiles) noexcept :
	Molecule(MolecularStructure(smiles))
{}

void Molecule::setDataStore(const DataStore& dataStore)
{
	dataAccessor.set(dataStore);
}

MoleculeId Molecule::getId() const
{
	return id;
}

Amount<Unit::GRAM_PER_MOLE> Molecule::getMolarMass() const
{
	return molarMass;
}

const MoleculeData& Molecule::data() const
{
	return dataAccessor.get().molecules.at(id);
}

const MolecularStructure& Molecule::getStructure() const
{
	return data().getStructure();
}

Polarity Molecule::getPolarity() const
{
	return this->data().polarity;
}

Color Molecule::getColor() const
{
	return this->data().color;
}

Amount<Unit::CELSIUS> Molecule::getMeltingPointAt(const Amount<Unit::TORR> pressure) const
{
	return this->data().meltingPointEstimator.get(pressure.asStd());
}

Amount<Unit::CELSIUS> Molecule::getBoilingPointAt(const Amount<Unit::TORR> pressure) const
{
	return this->data().boilingPointEstimator.get(pressure.asStd());
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
	const auto& data = this->data();
	return
		aggregation == AggregationType::GAS ? Formulas::idealGasLaw(temperature, pressure, molarMass) :
		aggregation == AggregationType::LIQUID ? data.liquidDensityEstimator.get(temperature.asStd()) :
		data.solidDensityEstimator.get(temperature.asStd());
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
	const auto& data = this->data();
	return
		aggregation == AggregationType::GAS ? Formulas::isobaricHeatCapacity(data.getStructure().getDegreesOfFreedom()) :
		aggregation == AggregationType::LIQUID ? data.liquidHeatCapacityEstimator.get(pressure.asStd()) :
		data.solidHeatCapacityEstimator.get(pressure.asStd());
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
	return this->data().fusionLatentHeatEstimator.get(temperature.asStd(), -pressure.asStd());
}

Amount<Unit::JOULE_PER_MOLE> Molecule::getVaporizationHeatAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	return this->data().vaporizationLatentHeatEstimator.get(temperature.asStd(), pressure.asStd());
}

Amount<Unit::JOULE_PER_MOLE> Molecule::getSublimationHeatAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	return this->data().sublimationLatentHeatEstimator.get(temperature.asStd(), pressure.asStd());
}

Amount<Unit::JOULE_PER_MOLE> Molecule::getLiquefactionHeatAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	return -getFusionHeatAt(temperature, pressure.asStd());
}

Amount<Unit::JOULE_PER_MOLE> Molecule::getCondensationHeatAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	return -getVaporizationHeatAt(temperature, -pressure.asStd());
}

Amount<Unit::JOULE_PER_MOLE> Molecule::getDepositionHeatAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	return -getSublimationHeatAt(temperature, -pressure.asStd());
}

Amount<Unit::MOLE_RATIO> Molecule::getSolubilityAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure,
	const Polarity& solventPolarity
) const
{
	const auto& solute = this->data();
	if (this->getAggregationAt(temperature, pressure) == AggregationType::GAS)
		return pressure.asStd() / solute.henrysConstantEstimator.get(temperature.asStd());

	// Direct approach
	//
	const auto baseSolubility =
		(solute.polarity.hydrophilicity * solventPolarity.hydrophilicity + solute.polarity.lipophilicity * solventPolarity.lipophilicity) /
		(solventPolarity.hydrophilicity + solventPolarity.lipophilicity);

	// Logarithmic approach
	// 
	//const auto logP = solventPolarity.getPartitionCoefficient().asStd();
	//const auto phi = 2.5;
	//const auto baseSolubility =
	//    logP <= -phi ? solute.polarity.hydrophilicity :
	//	  logP >= phi ? solute.polarity.lipophilicity :
	//	  (solute.polarity.hydrophilicity * (phi - logP) + solute.polarity.lipophilicity * (phi + logP)) / (2.0 * phi);

	const auto scale = solute.relativeSolubilityEstimator.get(temperature.asStd());
	return baseSolubility * scale;
}

bool Molecule::operator==(const Molecule& other) const
{
	return this->id == other.id;
}

bool Molecule::operator!=(const Molecule& other) const
{
	return this->id != other.id;
}