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
		const EstimatorIdType id,
		double(*function)(InArgs...)
	) noexcept;

	double get(InArgs...) const override final;

	FunctionalEstimator* clone() const override final;
};


template <typename... InArgs>
FunctionalEstimator<InArgs...>::FunctionalEstimator(
	const EstimatorIdType id,
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
FunctionalEstimator<InArgs...>* FunctionalEstimator<InArgs...>::clone() const
{
	return new FunctionalEstimator<InArgs...>(*this);
}