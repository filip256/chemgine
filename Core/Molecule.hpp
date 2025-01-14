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
	Quantity<GramPerMole> molarMass;

public:
	Molecule(const MoleculeData& data) noexcept;
	Molecule(MolecularStructure&& structure) noexcept;
	Molecule(const std::string& smiles) noexcept;
	Molecule(const Molecule&) = default;

	Molecule& operator=(const Molecule&) = default;

	MoleculeId getId() const;
	Quantity<GramPerMole> getMolarMass() const;
	const MoleculeData& getData() const;

	const MolecularStructure& getStructure() const;

	Polarity getPolarity() const;
	Color getColor() const;

	Quantity<AbsCelsius> getMeltingPointAt(const Quantity<Torr> pressure) const;
	Quantity<AbsCelsius> getBoilingPointAt(const Quantity<Torr> pressure) const;

	AggregationType getAggregationAt(
		const Quantity<AbsCelsius> temperature,
		const Quantity<Torr> pressure
	) const;

	Quantity<GramPerMilliLiter> getDensityAt(
		const Quantity<AbsCelsius> temperature,
		const Quantity<Torr> pressure,
		const AggregationType aggregation
	) const;

	Quantity<GramPerMilliLiter> getDensityAt(
		const Quantity<AbsCelsius> temperature,
		const Quantity<Torr> pressure
	) const;

	Quantity<JoulePerMoleCelsius> getHeatCapacityAt(
		const Quantity<AbsCelsius> temperature,
		const Quantity<Torr> pressure,
		const AggregationType aggregation
	) const;

	Quantity<JoulePerMoleCelsius> getHeatCapacityAt(
		const Quantity<AbsCelsius> temperature,
		const Quantity<Torr> pressure
	) const;

	Quantity<JoulePerMole> getFusionHeatAt(
		const Quantity<AbsCelsius> temperature,
		const Quantity<Torr> pressure
	) const;

	Quantity<JoulePerMole> getVaporizationHeatAt(
		const Quantity<AbsCelsius> temperature,
		const Quantity<Torr> pressure
	) const;

	Quantity<JoulePerMole> getSublimationHeatAt(
		const Quantity<AbsCelsius> temperature,
		const Quantity<Torr> pressure
	) const;

	Quantity<JoulePerMole> getLiquefactionHeatAt(
		const Quantity<AbsCelsius> temperature,
		const Quantity<Torr> pressure
	) const;

	Quantity<JoulePerMole> getCondensationHeatAt(
		const Quantity<AbsCelsius> temperature,
		const Quantity<Torr> pressure
	) const;

	Quantity<JoulePerMole> getDepositionHeatAt(
		const Quantity<AbsCelsius> temperature,
		const Quantity<Torr> pressure
	) const;

	Quantity<MoleRatio> getSolubilityAt(
		const Quantity<AbsCelsius> temperature,
		const Quantity<Torr> pressure,
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
