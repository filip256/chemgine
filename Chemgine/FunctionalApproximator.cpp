#include "FunctionalApproximator.hpp"

FunctionalApproximator::FunctionalApproximator(
	const ApproximatorIdType id,
	double(*function)(double)
) noexcept :
	BaseApproximator(id),
	function(function)
{}

double FunctionalApproximator::get(const double input) const
{
	return function(input);
}

FunctionalApproximator* FunctionalApproximator::clone() const
{
	return new FunctionalApproximator(*this);
}