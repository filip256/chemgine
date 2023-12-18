#pragma once

#include "BaseApproximator.hpp"

class SplitApproximator : public BaseApproximator
{
private:
	const double threshold;
	const BaseApproximator& lower;
	const BaseApproximator& higher;

public:
	SplitApproximator(
		const ApproximatorIdType id,
		const std::string& name,
		const BaseApproximator& lower,
		const BaseApproximator& higher,
		const double threshold
	) noexcept;

	double get(const double input) const override final;

	SplitApproximator* clone() const override final;
};