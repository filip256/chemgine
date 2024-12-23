#pragma once

#include "PortType.hpp"
#include "DynamicAmount.hpp"
#include "Keywords.hpp"

class LabwarePort
{
public:
	const PortType type;
	const float_s x, y;
	const Amount<Unit::DEGREE> angle;

	LabwarePort(
		const PortType type,
		const float_s x, const float_s y,
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

		const auto xIt = props->find(Def::Port::X);
		const auto yIt = props->find(Def::Port::Y);
		const auto angleIt = props->find(Def::Port::Angle);

		if (xIt == props->end() || yIt == props->end() || angleIt == props->end())
			return std::nullopt;

		const auto x = Def::parse<float_s>(xIt->second);
		const auto y = Def::parse<float_s>(yIt->second);
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
			{ Def::Port::X, Def::print(object.x) },
			{ Def::Port::Y, Def::print(object.y) },
			{ Def::Port::Angle, Def::print(object.angle) }
		};

		return Def::print(std::pair(Def::print(object.type), std::move(props)));
	}

	static std::string prettyPrint(const LabwarePort& object)
	{
		std::unordered_map<std::string, std::string> props
		{
			{ Def::Port::X, Def::print(object.x) },
			{ Def::Port::Y, Def::print(object.y) },
			{ Def::Port::Angle, Def::print(object.angle) }
		};

		return Def::prettyPrint(std::pair(Def::prettyPrint(object.type), std::move(props)));
	}
};
