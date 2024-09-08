#pragma once

#include "PortType.hpp"
#include "DynamicAmount.hpp"

class LabwarePort
{
public:
	const PortType type;
	const float x, y;
	const Amount<Unit::DEGREE> angle;

	LabwarePort(
		const PortType type,
		const float x, const float y,
		const Amount<Unit::DEGREE> angle
	) noexcept;
};


template <>
class Def::Parser<LabwarePort>
{
public:
	static std::optional<LabwarePort> parse(const std::string& str)
	{
		const auto port = Utils::split(str, ':', true);

		if (port.size() != 4)
			return std::nullopt;

		const auto type = Def::parseEnum<PortType>(port[0]);
		const auto x = Def::parse<uint32_t>(port[1]);
		const auto y = Def::parse<uint32_t>(port[2]);
		const auto angle = Def::parse<Amount<Unit::DEGREE>>(port[3]);

		if (type.has_value() == false || x.has_value() == false || y.has_value() == false || angle.has_value() == false)
			return std::nullopt;

		return LabwarePort(*type, *x, *y, *angle);
	}
};
