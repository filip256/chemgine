#pragma once

#include "Reactant.hpp"

#include <unordered_set>

class Mixture;

class ReactantSet
{
private:
	Ref<Mixture> container;
	std::unordered_set<Reactant> content;

	ReactantSet(
		const ReactantSet& other,
		const Ref<Mixture> newContainer
	) noexcept;

public:
	ReactantSet(const Ref<Mixture> container) noexcept;
	ReactantSet(ReactantSet&&) = default;
	ReactantSet(const std::vector<Molecule>& content) noexcept;
	ReactantSet(const std::vector<Reactant>& content) noexcept;

	size_t size() const;
	void reserve(const size_t size);

	bool contains(const Reactant& reactant) const;

	void add(const Reactant& reactant);

	/// <summary>
	/// Returns o reactant from the set.
	/// This is equivalent to dereferencing the begin operator of the underlying unordered_set.
	/// </summary>
	const Reactant& any() const;

	Amount<Unit::MOLE> getAmountOf(const Reactant& reactant) const;
	Amount<Unit::MOLE> getAmountOf(const ReactantSet& reactantSet) const;

	std::unordered_set<Reactant>::const_iterator erase(
		const std::unordered_set<Reactant>::const_iterator it);
	void erase(bool (*predicate)(const Reactant&));

	std::unordered_set<Reactant>::const_iterator begin() const;
	std::unordered_set<Reactant>::const_iterator end() const;

	bool equals(const ReactantSet& other,
		const Amount<>::StorageType epsilon = std::numeric_limits<Amount<>::StorageType>::epsilon()) const;

	bool operator==(const ReactantSet& other) const;
	bool operator!=(const ReactantSet& other) const;

	ReactantSet makeCopy(const Ref<Mixture> newContainer) const;
};