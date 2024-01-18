#include "Molecule.hpp"
#include "DataStore.hpp"
#include "Amount.hpp"
#include "Formulas.hpp"

DataStoreAccessor Molecule::dataAccessor = DataStoreAccessor();

Molecule::Molecule(const MoleculeIdType id) noexcept:
	id(id),
	molarMass(dataAccessor.get().molecules.at(id).getStructure().getMolarMass())
{
	dataAccessor.crashIfUninitialized();
}

Molecule::Molecule(MolecularStructure&& structure) noexcept:
	id(dataAccessor.getSafe().molecules.findOrAdd(std::move(structure)))
{
	molarMass = data().getStructure().getMolarMass();
}

Molecule::Molecule(const std::string& smiles) noexcept:
	Molecule(MolecularStructure(smiles))
{
}

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
	return this->data().meltingPointApproximator.get(pressure.asStd());
}

Amount<Unit::CELSIUS> Molecule::getBoilingPointAt(const Amount<Unit::TORR> pressure) const
{
	return this->data().boilingPointApproximator.get(pressure.asStd());
}

Amount<Unit::GRAM_PER_MILLILITER> Molecule::getDensityAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	const auto& data = this->data();
	return temperature < getMeltingPointAt(pressure) ?
		data.solidDensityApproximator.get(temperature.asStd()) :
		temperature < getBoilingPointAt(pressure) ?
			data.liquidDensityApproximator.get(temperature.asStd()) :
			Formulas::idealGasLaw(temperature, pressure, molarMass);
}

Amount<Unit::JOULE_PER_MOLE> Molecule::getHeatCapacityAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	const auto& data = this->data();
	return temperature < getMeltingPointAt(pressure) ?
		data.solidHeatCapacityApproximator.get(pressure.asStd()) :
		temperature < getBoilingPointAt(pressure) ?
			data.liquidHeatCapacityApproximator.get(pressure.asStd()) :
			Formulas::isobaricHeatCapacity(data.getStructure().getDegreesOfFreedom());
}

Amount<Unit::JOULE_PER_MOLE> Molecule::getFusionHeatAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	return this->data().fusionLatentHeatApproximator.get(temperature.asStd(), -pressure.asStd());
}

Amount<Unit::JOULE_PER_MOLE> Molecule::getVaporizationHeatAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	return this->data().vaporizationLatentHeatApproximator.get(temperature.asStd(), pressure.asStd());
}

Amount<Unit::JOULE_PER_MOLE> Molecule::getSublimationHeatAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	return this->data().sublimationLatentHeatApproximator.get(temperature.asStd(), pressure.asStd());
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


size_t MoleculeHash::operator() (const Molecule& molecule) const
{
	return std::hash<MoleculeIdType>()(molecule.getId());
}