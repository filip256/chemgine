#pragma once

#include "molecules/kinds/Molecule.hpp"
#include "reactions/ReactantSet.hpp"

#include <unordered_map>

class Mixture;

class ContentInitializer
{
private:
	std::unordered_map<Molecule, Amount<Unit::MOLE>> content;

public:
	ContentInitializer(
		std::initializer_list<std::pair<Molecule, Amount<Unit::MOLE>>> content
	) noexcept;
	ContentInitializer(const ReactantSet& content) noexcept;
	ContentInitializer() = default;
	ContentInitializer(ContentInitializer&&) = default;

	using const_iterator = std::unordered_map<const Molecule, Amount<Unit::MOLE>>::const_iterator;

	size_t size() const;

	void add(const Molecule& molecule, Amount<Unit::MOLE> amount);

	const_iterator begin() const;
	const_iterator end() const;
};
