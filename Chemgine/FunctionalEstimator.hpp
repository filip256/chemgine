#pragma once

#include "BaseEstimator.hpp"

#include <string>

template <typename... InArgs>
class FunctionalEstimator : public BaseEstimator
{
private:
	double (*const function)(InArgs...);

public:
	FunctionalEstimator(
		const EstimatorId id,
		double(*function)(InArgs...)
	) noexcept;

	double get(InArgs...) const override final;

	bool isEquivalent(const BaseEstimator& other,
		const double epsilon = std::numeric_limits<double>::epsilon()
	) const override final;

	FunctionalEstimator* clone() const override final;
};


template <typename... InArgs>
FunctionalEstimator<InArgs...>::FunctionalEstimator(
	const EstimatorId id,
	double(*function)(InArgs...)
) noexcept :
	BaseEstimator(id),
	function(function)
{}

template <typename... InArgs>
double FunctionalEstimator<InArgs...>::get(InArgs... inArgs) const
{
	return function(std::forward<InArgs>(inArgs)...);
}

template <typename... InArgs>
bool FunctionalEstimator<InArgs...>::isEquivalent(const BaseEstimator& other, const double epsilon) const
{
	return false; // functionals cannot be compared
}

template <typename... InArgs>
FunctionalEstimator<InArgs...>* FunctionalEstimator<InArgs...>::clone() const
{
	return new FunctionalEstimator<InArgs...>(*this);
}
