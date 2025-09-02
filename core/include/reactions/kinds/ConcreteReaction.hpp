#pragma once

#include "reactions/ReactantSet.hpp"
#include "reactions/data/ReactionData.hpp"
#include "utils/Hash.hpp"

class ConcreteReaction
{
private:
    const ReactionData& baseReaction;
    ReactantSet         reactants;
    ReactantSet         products;

    ConcreteReaction(const ConcreteReaction& other) noexcept;

public:
    ConcreteReaction(
        const ReactionData&          baseReaction,
        const std::vector<Reactant>& reactants,
        const std::vector<Molecule>& products) noexcept;

    ConcreteReaction(ConcreteReaction&&) = default;

    const ReactantSet&            getReactants() const;
    const ReactantSet&            getProducts() const;
    const ImmutableSet<Catalyst>& getCatalysts() const;

    const Amount<Unit::CELSIUS> getReactantTemperature() const;

    const ReactionData& getData() const;

    std::string getHRTag() const;

    bool operator==(const ConcreteReaction& other) const;
    bool operator!=(const ConcreteReaction& other) const;

    ConcreteReaction makeCopy() const;

    friend struct std::hash<ConcreteReaction>;
};

template <>
struct std::hash<ConcreteReaction>
{
    size_t operator()(const ConcreteReaction& reaction) const
    {
        size_t hash = 0;
        for (const auto& [rId, _] : reaction.reactants)
            utils::hashCombineWith(hash, rId);
        for (const auto& [pId, _] : reaction.products)
            utils::hashCombineWith(hash, pId);
        for (const auto& cat : reaction.baseReaction.getCatalysts())
            utils::hashCombineWith(hash, cat.getId());
        return hash;
    }
};
