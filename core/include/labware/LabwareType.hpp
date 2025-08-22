#pragma once

#include "data/def/Keywords.hpp"
#include "data/def/Parsers.hpp"
#include "data/def/Printers.hpp"

#include <cstdint>

enum class LabwareType : uint8_t
{
    NONE           = 0,
    FLASK          = 1,
    ADAPTOR        = 2,
    CONDENSER      = 3,
    FUNNEL         = 4,
    SEP_FUNNEL     = 5,
    DROP_FUNNEL    = 6,
    HEATSOURCE     = 7,
    STIRER         = 8,
    HEATER_STIRRER = 9
};

static inline constexpr bool hasMultiLayerStorage(const LabwareType type)
{
    return type == LabwareType::FLASK ||
           type == LabwareType::SEP_FUNNEL ||
           type == LabwareType::DROP_FUNNEL;
}

template <>
class def::Parser<LabwareType>
{
public:
    static std::optional<LabwareType> parse(const std::string& str)
    {
        static const std::unordered_map<std::string, LabwareType> typeMap{
            {     def::Labware::Flask,      LabwareType::FLASK},
            {   def::Labware::Adaptor,    LabwareType::ADAPTOR},
            { def::Labware::Condenser,  LabwareType::CONDENSER},
            {def::Labware::Heatsource, LabwareType::HEATSOURCE},
        };

        return utils::find(typeMap, str);
    }
};

template <>
class def::Printer<LabwareType>
{
public:
    static std::string print(const LabwareType object)
    {
        static const std::unordered_map<LabwareType, std::string> typeMap{
            {     LabwareType::FLASK,      def::Labware::Flask},
            {   LabwareType::ADAPTOR,    def::Labware::Adaptor},
            { LabwareType::CONDENSER,  def::Labware::Condenser},
            {LabwareType::HEATSOURCE, def::Labware::Heatsource},
        };

        return typeMap.at(object);
    }
};
