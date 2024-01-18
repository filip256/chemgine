#pragma once

#include "BaseApproximator.hpp"

#include <string>

template <typename... InArgs>
class FunctionalApproximator : public BaseApproximator
{
private:
	double (*const function)(InArgs...);

public:
	FunctionalApproximator(
		const ApproximatorIdType id,
		double(*function)(InArgs...)
	) noexcept;

	double get(InArgs...) const override final;

	FunctionalApproximator* clone() const override final;
};


template <typename... InArgs>
FunctionalApproximator<InArgs...>::FunctionalApproximator(
	const ApproximatorIdType id,
	double(*function)(InArgs...)
) noexcept :
	BaseApproximator(id),
	function(function)
{}

template <typename... InArgs>
double FunctionalApproximator<InArgs...>::get(InArgs... inArgs) const
{
	return function(std::forward<InArgs>(inArgs)...);
}

template <typename... InArgs>
FunctionalApproximator<InArgs...>* FunctionalApproximator<InArgs...>::clone() const
{
	return new FunctionalApproximator<InArgs...>(*this);
}