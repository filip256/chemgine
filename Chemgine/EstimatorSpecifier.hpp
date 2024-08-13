#pragma once

#include "Unit.hpp"

class EstimatorSpecifier
{
public:
	const Unit inUnit;
	const Unit outUnit;

	EstimatorSpecifier(
		const Unit inUnit,
		const Unit outUnit
	) noexcept;
	EstimatorSpecifier(const EstimatorSpecifier&) = delete;
	EstimatorSpecifier(EstimatorSpecifier&&) = default;
};
