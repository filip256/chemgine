#pragma once

#include "PortType.hpp"
#include "data/def/Keywords.hpp"
#include "data/values/DynamicAmount.hpp"

class LabwarePort
{
public:
    const PortType             type;
    const float_s              x, y;
    const Amount<Unit::DEGREE> angle;

    LabwarePort(const PortType type, const float_s x, const float_s y, const Amount<Unit::DEGREE> angle) noexcept;
};

template <>
class def::Parser<LabwarePort>
{
public:
    static std::optional<LabwarePort> parse(const std::string& str)
    {
        const auto pair = def::parse<std::pair<PortType, std::string>>(str);
        if (not pair)
            return std::nullopt;

        const auto props = def::parse<utils::StringMap<std::string>>(pair->second);
        if (not props)
            return std::nullopt;

        const auto xIt     = props->find(def::Port::X);
        const auto yIt     = props->find(def::Port::Y);
        const auto angleIt = props->find(def::Port::Angle);

        if (xIt == props->end() || yIt == props->end() || angleIt == props->end())
            return std::nullopt;

        const auto x     = def::parse<float_s>(xIt->second);
        const auto y     = def::parse<float_s>(yIt->second);
        const auto angle = def::parse<Amount<Unit::DEGREE>>(angleIt->second);

        if (not(x && y && angle))
            return std::nullopt;

        return LabwarePort(pair->first, *x, *y, *angle);
    }
};

template <>
class def::Printer<LabwarePort>
{
public:
    static std::string print(const LabwarePort& object)
    {
        std::unordered_map<std::string_view, std::string> props{
            {    def::Port::X,     def::print(object.x)},
            {    def::Port::Y,     def::print(object.y)},
            {def::Port::Angle, def::print(object.angle)}
        };

        return def::print(std::pair(def::print(object.type), std::move(props)));
    }

    static std::string prettyPrint(const LabwarePort& object)
    {
        std::unordered_map<std::string_view, std::string> props{
            {    def::Port::X,     def::print(object.x)},
            {    def::Port::Y,     def::print(object.y)},
            {def::Port::Angle, def::print(object.angle)}
        };

        return def::prettyPrint(std::pair(def::prettyPrint(object.type), std::move(props)));
    }
};
