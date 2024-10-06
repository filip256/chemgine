#pragma once

#include "UnitizedEstimator.hpp"

template<Unit OutU, Unit... InUs>
class DerivedEstimator : public UnitizedEstimator<OutU, InUs...>
{
private:
	virtual void tryPrintOOLDefinition(
		std::ostream& out,
		std::unordered_set<EstimatorId>& alreadyPrinted
	) const = 0;

	virtual void printILDefinition(
		std::ostream& out,
		std::unordered_set<EstimatorId>& alreadyPrinted
	) const = 0;

public:
	using UnitizedEstimator<OutU, InUs...>::UnitizedEstimator;

	void printDefinition(
		std::ostream& out,
		std::unordered_set<EstimatorId>& alreadyPrinted,
		const bool printInline
	) const override final;
};


template<Unit OutU, Unit... InUs>
void DerivedEstimator<OutU, InUs...>::printDefinition(
	std::ostream& out, std::unordered_set<EstimatorId>& alreadyPrinted, const bool printInline) const
{
	return printInline ?
		printILDefinition(out, alreadyPrinted) :
		tryPrintOOLDefinition(out, alreadyPrinted);
}
