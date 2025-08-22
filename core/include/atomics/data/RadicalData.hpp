#pragma once

#include "atomics/data/AtomData.hpp"

#include <unordered_set>

class AtomRepository;

class RadicalData : public AtomData
{
public:
    const std::unordered_set<Symbol> matchables;

    RadicalData(
        const Symbol&                symbol,
        const std::string&           name,
        std::unordered_set<Symbol>&& matchables) noexcept;

    void dumpDefinition(std::ostream& out, const bool prettify) const override final;

    static const std::unordered_set<Symbol> MatchAny;
};
