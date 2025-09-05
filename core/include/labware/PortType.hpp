#pragma once

#include "data/def/Parsers.hpp"
#include "data/def/Printers.hpp"

#include <cstdint>

enum class PortType : uint8_t
{
    NONE    = 0,
    SIMPLE  = 1,
    INLET   = 2,
    OUTLET  = 3,
    DROPPER = 4,
    HOSE    = 5,
    CONTACT = 6
};

template <>
class def::Parser<PortType>
{
public:
    static std::optional<PortType> parse(const std::string& str)
    {
        static const std::unordered_map<std::string, PortType> typeMap{
            { "SIMPLE",  PortType::SIMPLE},
            {  "INLET",   PortType::INLET},
            { "OUTLET",  PortType::OUTLET},
            {"DROPPER", PortType::DROPPER},
            {   "HOSE",    PortType::HOSE},
            {"CONTACT", PortType::CONTACT},
        };

        return utils::find(typeMap, str);
    }
};

template <>
class def::Printer<PortType>
{
public:
    static std::string print(const PortType object)
    {
        static const std::unordered_map<PortType, std::string> typeMap{
            {PortType::SIMPLE, "SIMPLE"},
            {PortType::INLET, "INLET"},
            {
             PortType::OUTLET,
             "OUTLET", },
            {PortType::DROPPER, "DROPPER"},
            {PortType::HOSE, "HOSE"},
            {PortType::CONTACT, "CONTACT"},
        };

        return typeMap.at(object);
    }
};
