#pragma once

#include "Reactant.hpp"

#include <unordered_set>

class ReactantSet
{
private:
	std::unordered_set<Reactant> content;

public:
	ReactantSet() = default;
	ReactantSet(const ReactantSet&) = delete;
	ReactantSet(ReactantSet&&) = default;
	ReactantSet(const std::vector<Molecule>& content) noexcept;
	ReactantSet(const std::vector<Reactant>& content) noexcept;

	bool contains(const Reactant& reactant) const;

	void add(const Reactant& reactant);

	/// <summary>
	/// Returns o reactant from the set.
	/// This is equivalent to dereferencing the begin operator of the underlying unordered_set.
	/// </summary>
	const Reactant& any() const;

	std::unordered_set<Reactant>::const_iterator begin() const;
	std::unordered_set<Reactant>::const_iterator end() const;
};