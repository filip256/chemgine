#pragma once

#include "reactions/data/ReactionData.hpp"
#include "utils/Hash.hpp"

#include <unordered_map>

class RetrosynthReaction
{
private:
    const ReactionData&                       baseReaction;
    std::unordered_map<StructureRef, uint8_t> reactants;
    std::unordered_map<StructureRef, uint8_t> products;

public:
    RetrosynthReaction(
        const ReactionData&              baseReaction,
        const std::vector<StructureRef>& reactants,
        const std::vector<StructureRef>& products) noexcept;

    RetrosynthReaction(const RetrosynthReaction&) = delete;
    RetrosynthReaction(RetrosynthReaction&&)      = default;

    const std::unordered_map<StructureRef, uint8_t>& getReactants() const;
    const std::unordered_map<StructureRef, uint8_t>& getProducts() const;

    const ReactionData& getBaseData() const;

    void print(std::ostream& out = std::cout) const;

    bool operator==(const RetrosynthReaction& other) const;
    bool operator!=(const RetrosynthReaction& other) const;

    friend struct std::hash<RetrosynthReaction>;
};

template <>
struct std::hash<RetrosynthReaction>
{
    size_t operator()(const RetrosynthReaction& reaction) const
    {
        size_t hash = 0;
        for (const auto& [r, _] : reaction.reactants)
            utils::hashCombineWith(hash, r);
        for (const auto& [r, _] : reaction.products)
            utils::hashCombineWith(hash, r);
        return hash;
    }
};
