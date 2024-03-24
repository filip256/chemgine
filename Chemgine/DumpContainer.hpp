#pragma once

#include "BaseContainer.hpp"
#include "Amount.hpp"

/// <summary>
/// Reactant container with no storage or properties. Just a dump for reactants.
/// </summary>
class DumpContainer final : public BaseContainer
{
private:
	Amount<Unit::GRAM> totalMass = 0.0;

public:
	void add(const Reactant& reactant) override final;

	Amount<Unit::GRAM> getTotalMass() const;

	static DumpContainer GlobalDumpContainer;
};
