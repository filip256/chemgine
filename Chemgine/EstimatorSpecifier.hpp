#pragma once

#include "DynamicAmount.hpp"

#include <vector>

class EstimatorSpecifier
{
public:
	const Unit outUnit;
	const std::vector<Unit> inUnits;

	EstimatorSpecifier(
		const Unit outUnit,
		std::vector<Unit>&& inUnits
	) noexcept;
	EstimatorSpecifier(const EstimatorSpecifier&) = delete;
	EstimatorSpecifier(EstimatorSpecifier&&) = default;
};


template <>
class Def::Parser<EstimatorSpecifier>
{
public:
	static std::optional<EstimatorSpecifier> parse(const std::string& str)
	{
		const auto sep = str.find("->");
		if (sep > str.size() - 3)
			return std::nullopt;

		const auto outUnit = Def::parse<Unit>(str.substr(sep + 2));
		if (not outUnit.has_value())
			return std::nullopt;

		auto inUnits = Def::parse<std::vector<Unit>>(str.substr(0, sep));
		if (not inUnits.has_value())
			return std::nullopt;

		return EstimatorSpecifier(*outUnit, std::move(*inUnits));
	}
};
