#pragma once

#include "Reactant.hpp"

#include <unordered_map>

class Mixture;

class ReactantSet
{
private:
	Ref<Mixture> container;
	std::unordered_map<ReactantId, Reactant> reactants;

	ReactantSet(
		const ReactantSet& other,
		const Ref<Mixture> newContainer
	) noexcept;

public:
	ReactantSet(const Ref<Mixture> container) noexcept;
	ReactantSet(ReactantSet&&) = default;
	ReactantSet(const std::vector<Molecule>& reactants) noexcept;
	ReactantSet(const std::vector<Reactant>& reactants) noexcept;

	using pairT = std::pair<ReactantId, Reactant>;
	using const_iterator = std::unordered_map<ReactantId, Reactant>::const_iterator;
	using iterator = std::unordered_map<ReactantId, Reactant>::iterator;

	size_t size() const;
	void reserve(const size_t size);

	bool contains(const ReactantId& reactantId) const;

	void add(const Reactant& reactant);

	/// <summary>
	/// Returns o reactant from the set.
	/// This is equivalent to dereferencing the begin operator of the underlying unordered_set.
	/// </summary>
	const Reactant& any() const;

	Amount<Unit::MOLE> getAmountOf(const ReactantId& reactantId) const;
	Amount<Unit::MOLE> getAmountOf(const ReactantSet& reactantSet) const;

	iterator erase(const iterator it);
	void erase(bool (*predicate)(const pairT&));

	const_iterator begin() const;
	iterator begin();
	const_iterator end() const;
	iterator end();

	bool equals(const ReactantSet& other,
		const Amount<>::StorageType epsilon = std::numeric_limits<Amount<>::StorageType>::epsilon()) const;

	bool operator==(const ReactantSet& other) const;
	bool operator!=(const ReactantSet& other) const;

	ReactantSet makeCopy(const Ref<Mixture> newContainer) const;
};
