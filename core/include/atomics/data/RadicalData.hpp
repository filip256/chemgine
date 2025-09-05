#pragma once

#include "atomics/data/AtomData.hpp"

#include <unordered_set>

class RadicalData : public AtomData
{
private:
    std::unordered_set<Symbol> matches;

public:
    RadicalData(const Symbol& symbol, const std::string& name, std::unordered_set<Symbol>&& matches) noexcept;

    const std::unordered_set<Symbol>& getMatches() const;

    void dumpDefinition(std::ostream& out, const bool prettify) const override final;

    static const std::unordered_set<Symbol> MatchAny;
};
