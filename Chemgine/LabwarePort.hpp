#pragma once

#include "PortType.hpp"
#include "DynamicAmount.hpp"
#include "Keywords.hpp"

class LabwarePort
{
public:
	const PortType type;
	const float_n x, y;
	const Amount<Unit::DEGREE> angle;

	LabwarePort(
		const PortType type,
		const float_n x, const float_n y,
		const Amount<Unit::DEGREE> angle
	) noexcept;
};


template <>
class Def::Parser<LabwarePort>
{
public:
	static std::optional<LabwarePort> parse(const std::string& str)
	{
		const auto pair = Def::parse<std::pair<PortType, std::string>>(str);
		if (not pair)
			return std::nullopt;

		const auto props = Def::parse<std::unordered_map<std::string, std::string>>(pair->second);
		if (not props)
			return std::nullopt;

		const auto xIt = props->find(Keywords::Port::X);
		const auto yIt = props->find(Keywords::Port::Y);
		const auto angleIt = props->find(Keywords::Port::Angle);

		if (xIt == props->end() || yIt == props->end() || angleIt == props->end())
			return std::nullopt;

		const auto x = Def::parse<uint32_t>(xIt->second);
		const auto y = Def::parse<uint32_t>(yIt->second);
		const auto angle = Def::parse<Amount<Unit::DEGREE>>(angleIt->second);

		if (not (x && y && angle))
			return std::nullopt;

		return LabwarePort(pair->first, *x, *y, *angle);
	}
};

template <>
class Def::Printer<LabwarePort>
{
public:
	static std::string print(const LabwarePort& object)
	{
		std::unordered_map<std::string, std::string> props
		{
			{ Keywords::Port::X, Def::print(object.x) },
			{ Keywords::Port::Y, Def::print(object.y) },
			{ Keywords::Port::Angle, Def::print(object.angle) }
		};

		return Def::print(std::pair(Def::print(object.type), std::move(props)));
	}
};
