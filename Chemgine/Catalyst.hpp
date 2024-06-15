#pragma once

#include "Reactable.hpp"
#include "Amount.hpp"

class Catalyst
{
private:
	const Amount<Unit::MOLE_RATIO> idealAmount;
	const Reactable reactable;

	Catalyst(const Reactable& reactable, const Amount<Unit::MOLE_RATIO> idealAmount) noexcept;

public:
	Catalyst(const Catalyst&) = default;

	const MoleculeId getId() const;
	const MolecularStructure& getStructure() const;
	const Amount<Unit::MOLE_RATIO> getIdealAmount() const;
	std::unordered_map<c_size, c_size> matchWith(const Catalyst& other) const;
	std::unordered_map<c_size, c_size> matchWith(const MolecularStructure& structure) const;
	bool matchesWith(const Catalyst& other) const;
	bool matchesWith(const MolecularStructure& structure) const;

	bool operator==(const Catalyst& other) const;
	bool operator!=(const Catalyst& other) const;

	static std::optional<Catalyst> get(
		const std::string& smiles,
		const Amount<Unit::MOLE_RATIO> idealAmount);

	friend struct std::hash<Catalyst>;
};


template<>
struct std::hash<Catalyst>
{
	size_t operator() (const Catalyst& catalyst) const
	{
		return std::hash<Reactable>()(catalyst.reactable);
	}
};

template<>
struct std::less<Catalyst>
{
	bool operator() (const Catalyst& x, const Catalyst& y) const
	{
		return std::hash<Catalyst>()(x) < std::hash<Catalyst>()(y);
	}
};
