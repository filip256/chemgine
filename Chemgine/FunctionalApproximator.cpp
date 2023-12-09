#include "FunctionalApproximator.hpp"

FunctionalApproximator::FunctionalApproximator(
	const ApproximatorIdType id,
	const std::string& name,
	double(*function)(double)
) noexcept :
	BaseApproximator(id, name, ApproximatorType::FUNCTIONAL),
	function(function)
{}

double FunctionalApproximator::execute(const double input) const
{
	return function(input);
}