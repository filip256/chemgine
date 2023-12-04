#include "FunctionalApproximator.hpp"

FunctionalApproximator::FunctionalApproximator(
	const ApproximatorIdType id,
	const std::string& name,
	double(*function)(double)
) noexcept :
	id(id),
	name(name),
	function(function)
{}

double FunctionalApproximator::execute(const double x) const
{
	return function(x);
}