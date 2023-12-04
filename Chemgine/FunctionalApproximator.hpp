#pragma once

#include <string>

typedef uint16_t ApproximatorIdType;

class FunctionalApproximator
{
private:
	double (*const function)(double);

public:
	const ApproximatorIdType id;
	const std::string name;

	FunctionalApproximator(
		const ApproximatorIdType id,
		const std::string& name,
		double(*function)(double)
	) noexcept;

	double execute(const double x) const;
};