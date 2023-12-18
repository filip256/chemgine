#include "FunctionalApproximator.hpp"

FunctionalApproximator::FunctionalApproximator(
	const ApproximatorIdType id,
	const std::string& name,
	double(*function)(double)
) noexcept :
	BaseApproximator(id, name, ApproximatorType::FUNCTIONAL),
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