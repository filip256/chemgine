#pragma once

#include "atomics/data/AtomBaseData.hpp"
#include "data/values/SymbolMatch.hpp"

class RadicalData : public AtomBaseData
{
private:
    SymbolMatchSet matches;

public:
    RadicalData(
        Symbol&&                          symbol,
        std::string&&                     name,
        const Amount<Unit::GRAM_PER_MOLE> weight,
        SymbolMatchSet&&                  matches) noexcept;

    const SymbolMatchSet& getMatches() const;
    void                  addInferredMatch(Symbol&& match);

    bool isRadical() const override final;

    const ImmutableSet<uint8_t>& getValences() const override final;
    uint8_t                      getFittingValence(const uint8_t bonds) const override final;
    bool                         hasValence(const uint8_t valence) const override final;

    uint8_t getPrecedence() const override final;

    void dumpDefinition(std::ostream& out, const bool prettify) const override final;

    static const ImmutableSet<uint8_t> AnyValence;
    static const SymbolMatchSet        MatchAny;
};
