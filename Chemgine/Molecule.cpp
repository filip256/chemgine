#include "Molecule.hpp"
#include "DataStore.hpp"
#include "Amount.hpp"
#include "Formulas.hpp"

DataStoreAccessor Molecule::dataAccessor = DataStoreAccessor();

Molecule::Molecule(const MoleculeIdType id) noexcept:
	id(id),
	molarMass(dataAccessor.getSafe().molecules.at(id).getStructure().getMolarMass())
{}

Molecule::Molecule(MolecularStructure&& structure) noexcept:
	id(dataAccessor.getSafe().molecules.findOrAdd(std::move(structure)))
{
	molarMass = data().getStructure().getMolarMass();
}

Molecule::Molecule(const std::string& smiles) noexcept:
	Molecule(MolecularStructure(smiles))
{}

void Molecule::setDataStore(const DataStore& dataStore)
{
	dataAccessor.set(dataStore);
}

MoleculeIdType Molecule::getId() const
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

const MolecularStructure& Molecule::getStructure() const
{
	return data().getStructure();
}

bool Molecule::operator==(const Molecule& other) const
{
	return this->id == other.id;
}

bool Molecule::operator!=(const Molecule& other) const
{
	return this->id != other.id;
}