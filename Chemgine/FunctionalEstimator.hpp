#pragma once

#include "EstimatorBase.hpp"

#include <string>

template <typename... InArgs>
class FunctionalEstimator : public EstimatorBase
{
private:
	double (*const function)(InArgs...);

public:
	FunctionalEstimator(
		const EstimatorId id,
		double(*function)(InArgs...)
	) noexcept;

	bool isEquivalent(const EstimatorBase& other,
		const double epsilon = std::numeric_limits<double>::epsilon()
	) const override final;

	FunctionalEstimator* clone() const override final;
};


template <typename... InArgs>
FunctionalEstimator<InArgs...>::FunctionalEstimator(
	const EstimatorId id,
	double(*function)(InArgs...)
) noexcept :
	EstimatorBase(id),
	function(function)
{}

template <typename... InArgs>
bool FunctionalEstimator<InArgs...>::isEquivalent(const EstimatorBase& other, const double epsilon) const
{
	return false; // functionals cannot be compared
}

template <typename... InArgs>
FunctionalEstimator<InArgs...>* FunctionalEstimator<InArgs...>::clone() const
{
	return new FunctionalEstimator<InArgs...>(*this);
}
