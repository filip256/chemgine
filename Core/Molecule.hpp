#pragma once

#include "DataStoreAccessor.hpp"
#include "AggregationType.hpp"
#include "MoleculeData.hpp"
#include "Parsers.hpp"
#include "Accessor.hpp"
#include "Amount.hpp"

class Molecule : public Accessor<>
{
private:
	const MoleculeData& data;
	Amount<Unit::GRAM_PER_MOLE> molarMass;

public:
	Molecule(const MoleculeData& data) noexcept;
	Molecule(MolecularStructure&& structure) noexcept;
	Molecule(const std::string& smiles) noexcept;
	Molecule(const Molecule&) = default;

	Molecule& operator=(const Molecule&) = default;

	MoleculeId getId() const;
	Amount<Unit::GRAM_PER_MOLE> getMolarMass() const;
	const MoleculeData& getData() const;

	const MolecularStructure& getStructure() const;

	Polarity getPolarity() const;
	Color getColor() const;

	Amount<Unit::CELSIUS> getMeltingPointAt(const Amount<Unit::TORR> pressure) const;
	Amount<Unit::CELSIUS> getBoilingPointAt(const Amount<Unit::TORR> pressure) const;

	AggregationType getAggregationAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure
	) const;

	Amount<Unit::GRAM_PER_MILLILITER> getDensityAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure,
		const AggregationType aggregation
	) const;

	Amount<Unit::GRAM_PER_MILLILITER> getDensityAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure
	) const;

	Amount<Unit::JOULE_PER_MOLE_CELSIUS> getHeatCapacityAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure,
		const AggregationType aggregation
	) const;

	Amount<Unit::JOULE_PER_MOLE_CELSIUS> getHeatCapacityAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure
	) const;

	Amount<Unit::JOULE_PER_MOLE> getFusionHeatAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure
	) const;

	Amount<Unit::JOULE_PER_MOLE> getVaporizationHeatAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure
	) const;

	Amount<Unit::JOULE_PER_MOLE> getSublimationHeatAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure
	) const;

	Amount<Unit::JOULE_PER_MOLE> getLiquefactionHeatAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure
	) const;

	Amount<Unit::JOULE_PER_MOLE> getCondensationHeatAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure
	) const;

	Amount<Unit::JOULE_PER_MOLE> getDepositionHeatAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure
	) const;

	Amount<Unit::MOLE_RATIO> getSolubilityAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure,
		const Polarity& solventPolarity
	) const;

	bool operator==(const Molecule& other) const;
	bool operator!=(const Molecule& other) const;
};

template<>
struct std::hash<Molecule>
{
	size_t operator() (const Molecule& molecule) const
	{
		return std::hash<MoleculeId>()(molecule.getData().id);
	}
};


template <>
class Def::Parser<Molecule>
{
public:
	static std::optional<Molecule> parse(const std::string& str)
	{
		return Def::parse<MolecularStructure>(str);
	}
};
