#pragma once

#include "DynamicAmount.hpp"

#include <vector>
#include <iostream>

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
		const auto stripped = Utils::strip(str);
		const auto sep = stripped.find("->");
		if (sep > stripped.size() - 3)
			return std::nullopt;

		const auto outUnit = Def::parse<Unit>(stripped.substr(sep + 2));
		if (not outUnit)
			return std::nullopt;

		const auto inUnitsStr = stripped.starts_with('(') ?
			stripped.substr(1, sep - 2) :
			stripped.substr(0, sep);

		auto inUnits = Def::parse<std::vector<Unit>>(inUnitsStr);
		if (not inUnits)
			return std::nullopt;

		return EstimatorSpecifier(*outUnit, std::move(*inUnits));
	}
};
