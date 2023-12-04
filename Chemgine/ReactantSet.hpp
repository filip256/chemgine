#pragma once

#include "Reactant.hpp"

#include <unordered_set>

class ReactantSet
{
private:
	std::unordered_set<Reactant, ReactantHash> content;

public:
	ReactantSet() = default;
	ReactantSet(const ReactantSet&) = delete;
	ReactantSet(ReactantSet&&) = default;
	ReactantSet(const std::vector<Molecule>& molecules) noexcept;

	bool contains(const Reactant& reactant) const;

	void add(const Reactant& reactant);

	std::unordered_set<Reactant, ReactantHash>::const_iterator begin() const;
	std::unordered_set<Reactant, ReactantHash>::const_iterator end() const;


};