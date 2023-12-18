#include "SplitApproximator.hpp"

SplitApproximator::SplitApproximator(
	const ApproximatorIdType id,
	const std::string& name,
	const BaseApproximator& lower,
	const BaseApproximator& higher,
	const double threshold
) noexcept :
	BaseApproximator(id, name, lower.type),
	lower(lower),
	higher(higher),
	threshold(threshold)
{}

double SplitApproximator::get(const double input) const
{
	return input < threshold ? lower.get(input) : higher.get(input);
}

SplitApproximator* SplitApproximator::clone() const
{
	return new SplitApproximator(*this);
}