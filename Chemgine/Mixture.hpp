#pragma once

#include "ReactantSet.hpp"

/// <summary>
/// The simplest type of reactant mixture.
/// <para> - Added reactants remain binded to their original container </para>
/// <para> - No physical properties </para>
/// </summary>
class Mixture
{
protected:
	ReactantSet content;

public:
	Mixture() = default;
	Mixture(const Mixture&) = delete;
	Mixture(Mixture&&) = default;

	virtual void add(const Reactant& reactant);

	Amount<Unit::MOLE> getAmountOf(const Reactant& reactant) const;
	Amount<Unit::MOLE> getAmountOf(const ReactantSet& reactantSet) const;

	friend class Reactant;
};