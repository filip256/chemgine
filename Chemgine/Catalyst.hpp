#pragma once

#include "Reactable.hpp"
#include "Amount.hpp"

class Catalyst
{
private:
	const Reactable reactable;
	const Amount<Unit::MOLAR_PERCENT> idealAmount;

	Catalyst(const Reactable& reactable, const Amount<Unit::MOLAR_PERCENT> idealAmount) noexcept;

public:
	Catalyst(const Catalyst&) = default;

	inline const MoleculeIdType getId() const;
	inline const MolecularStructure& getStructure() const;
	inline std::unordered_map<c_size, c_size> matchWith(const MolecularStructure& structure) const;

	static std::optional<Catalyst> get(
		const std::string& smiles,
		const Amount<Unit::MOLAR_PERCENT> idealAmount);
};