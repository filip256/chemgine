#pragma once

#include "data/def/Keywords.hpp"
#include "data/def/Parsers.hpp"
#include "data/def/Printers.hpp"

#include <cstdint>

class Color
{
public:
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 255;

    Color() = default;
    Color(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha) noexcept;
};

template <>
class def::Parser<Color>
{
public:
    static std::optional<::Color> parse(const std::string& str)
    {
        const auto props = def::parse<utils::StringMap<std::string>>(str);
        if (not props)
            return std::nullopt;

        const auto rIt         = props->find(def::Color::R);
        const auto gIt         = props->find(def::Color::G);
        const auto bIt         = props->find(def::Color::B);
        const auto intensityIt = props->find(def::Color::Intensity);

        if (rIt == props->end() || gIt == props->end() || bIt == props->end())
            return std::nullopt;

        const auto r         = def::parse<uint8_t>(rIt->second);
        const auto g         = def::parse<uint8_t>(gIt->second);
        const auto b         = def::parse<uint8_t>(bIt->second);
        const auto intensity = def::parse<uint8_t>(intensityIt->second);

        if (not(r && g && b && intensity))
            return std::nullopt;

        return ::Color(*r, *g, *b, *intensity);
    }
};

template <>
class def::Printer<Color>
{
public:
    static std::string print(const ::Color object)
    {
        std::unordered_map<std::string_view, uint8_t> props{
            {        def::Color::R, object.r},
            {        def::Color::G, object.g},
            {        def::Color::B, object.b},
            {def::Color::Intensity, object.a}
        };

        return def::print(props);
    }

    static std::string prettyPrint(const ::Color object)
    {
        std::unordered_map<std::string_view, uint8_t> props{
            {        def::Color::R, object.r},
            {        def::Color::G, object.g},
            {        def::Color::B, object.b},
            {def::Color::Intensity, object.a}
        };

        return def::prettyPrint(props);
    }
};
