#pragma once

#include "UnitizedEstimator.hpp"

template<Unit OutU, Unit... InUs>
class DerivedEstimator : public UnitizedEstimator<OutU, InUs...>
{
private:
	virtual void tryPrintOOLDefinition(
		std::ostream& out,
		const bool prettify,
		std::unordered_set<EstimatorId>& alreadyPrinted,
		const uint16_t baseIndent
	) const = 0;

	virtual void printILDefinition(
		std::ostream& out,
		const bool prettify,
		std::unordered_set<EstimatorId>& alreadyPrinted,
		const uint16_t baseIndent
	) const = 0;

public:
	using UnitizedEstimator<OutU, InUs...>::UnitizedEstimator;

	void dumpDefinition(
		std::ostream& out,
		const bool prettify,
		std::unordered_set<EstimatorId>& alreadyPrinted,
		const bool printInline,
		const uint16_t baseIndent
	) const override final;
};


template<Unit OutU, Unit... InUs>
void DerivedEstimator<OutU, InUs...>::dumpDefinition(
	std::ostream& out,
	const bool prettify,
	std::unordered_set<EstimatorId>& alreadyPrinted,
	const bool printInline,
	const uint16_t baseIndent) const
{
	return printInline ?
		printILDefinition(out, prettify, alreadyPrinted, baseIndent) :
		tryPrintOOLDefinition(out, prettify, alreadyPrinted, baseIndent);
}
