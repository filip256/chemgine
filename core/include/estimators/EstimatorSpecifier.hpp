#pragma once

#include "data/values/DynamicAmount.hpp"

#include <iostream>
#include <vector>

namespace def
{

class EstimatorSpecifier
{
public:
    const Unit              outUnit;
    const std::vector<Unit> inUnits;

    EstimatorSpecifier(const Unit outUnit, std::vector<Unit>&& inUnits) noexcept;
    EstimatorSpecifier(const EstimatorSpecifier&) = delete;
    EstimatorSpecifier(EstimatorSpecifier&&)      = default;
};

template <>
class Parser<EstimatorSpecifier>
{
public:
    static std::optional<EstimatorSpecifier> parse(const std::string& str)
    {
        const auto stripped = utils::strip(str);
        const auto sep      = stripped.find("->");
        if (sep > stripped.size() - 3)
            return std::nullopt;

        const auto outUnit = def::parse<Unit>(stripped.substr(sep + 2));
        if (not outUnit)
            return std::nullopt;

        auto inUnitsStr = utils::strip(stripped.substr(0, sep));
        if (inUnitsStr.starts_with('(') && inUnitsStr.ends_with(')'))
            inUnitsStr = inUnitsStr.substr(1, inUnitsStr.size() - 2);

        auto inUnits = def::parse<std::vector<Unit>>(inUnitsStr);
        if (not inUnits)
            return std::nullopt;

        return EstimatorSpecifier(*outUnit, std::move(*inUnits));
    }
};

template <>
class Printer<EstimatorSpecifier>
{
public:
    static std::string print(const EstimatorSpecifier& object)
    {
        std::string result;
        if (object.inUnits.size() == 1)
            result += DynamicAmount::getUnitSymbol(object.inUnits.front());
        else {
            result += '(';
            for (size_t i = 0; i < object.inUnits.size() - 1; ++i)
                result += DynamicAmount::getUnitSymbol(object.inUnits[i]) + ',';
            result += DynamicAmount::getUnitSymbol(object.inUnits.back());
            result += ')';
        }

        result += "->";
        result += DynamicAmount::getUnitSymbol(object.outUnit);
        return result;
    }

    static std::string prettyPrint(const EstimatorSpecifier& object)
    {
        std::string result;
        if (object.inUnits.size() == 1)
            result += DynamicAmount::getUnitSymbol(object.inUnits.front());
        else {
            result += '(';
            for (size_t i = 0; i < object.inUnits.size() - 1; ++i)
                result += DynamicAmount::getUnitSymbol(object.inUnits[i]) + ", ";
            result += DynamicAmount::getUnitSymbol(object.inUnits.back());
            result += ')';
        }

        result += " -> ";
        result += DynamicAmount::getUnitSymbol(object.outUnit);
        return result;
    }
};

}  // namespace def
