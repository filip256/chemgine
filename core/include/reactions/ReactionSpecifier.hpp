#pragma once

#include "data/def/Parsers.hpp"
#include "data/def/Printers.hpp"

#include <string>
#include <vector>

namespace def
{

class ReactionSpecifier
{
public:
    const std::vector<std::string> reactants;
    const std::vector<std::string> products;

    ReactionSpecifier(
        std::vector<std::string>&& reactants, std::vector<std::string>&& products) noexcept;
    ReactionSpecifier(const ReactionSpecifier&) = delete;
    ReactionSpecifier(ReactionSpecifier&&)      = default;
};

template <>
class Parser<ReactionSpecifier>
{
public:
    static std::optional<ReactionSpecifier> parse(const std::string& str)
    {
        const auto sep = str.find("->");
        if (sep > str.size() - 3)
            return std::nullopt;

        const auto reactantsStr = str.substr(0, sep);
        const auto productsStr  = str.substr(sep + 2);

        auto reactants = utils::split(reactantsStr, '+', false);
        auto products  = utils::split(productsStr, '+', false);

        if (reactants.empty() || products.empty())
            return std::nullopt;

        for (size_t i = 0; i < reactants.size(); ++i) {
            utils::strip(reactants[i]);
            if (reactants[i].empty())
                return std::nullopt;
        }

        for (size_t i = 0; i < products.size(); ++i) {
            utils::strip(products[i]);
            if (products[i].empty())
                return std::nullopt;
        }

        return std::optional<ReactionSpecifier>(
            std::in_place, std::move(reactants), std::move(products));
    }
};

template <>
class Printer<ReactionSpecifier>
{
public:
    static std::string print(const ReactionSpecifier& object)
    {
        std::string result;
        for (size_t i = 0; i < object.reactants.size() - 1; ++i)
            result += object.reactants[i] + "+";
        result += object.reactants.back();

        result += "->";

        for (size_t i = 0; i < object.products.size() - 1; ++i) result += object.products[i] + "+";
        result += object.products.back();
        return result;
    }

    static std::string prettyPrint(const ReactionSpecifier& object)
    {
        std::string result;
        for (size_t i = 0; i < object.reactants.size() - 1; ++i)
            result += object.reactants[i] + " + ";
        result += object.reactants.back();

        result += " -> ";

        for (size_t i = 0; i < object.products.size() - 1; ++i)
            result += object.products[i] + " + ";
        result += object.products.back();
        return result;
    }
};

}  // namespace def
