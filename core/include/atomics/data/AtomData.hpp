#pragma once

#include "atomics/data/AtomBaseData.hpp"

class AtomData : public AtomBaseData
{
private:
    const ImmutableSet<uint8_t> valences;
    const uint8_t               rarity;

public:
    AtomData(
        Symbol&&                          symbol,
        std::string&&                     name,
        const Amount<Unit::GRAM_PER_MOLE> weight,
        ImmutableSet<uint8_t>&&           valences) noexcept;

    bool isRadical() const override final;

    const ImmutableSet<uint8_t>& getValences() const override final;
    uint8_t                      getFittingValence(const uint8_t bonds) const override final;
    bool                         hasValence(const uint8_t valence) const override final;

    uint8_t getPrecedence() const override final;

    void dumpDefinition(std::ostream& out, const bool prettify) const override final;

    static const uint8_t NullValence = static_cast<uint8_t>(-1);
};
