#pragma once

#include "BondType.hpp"
#include "AtomIdentifier.hpp"

class GraphEdge
{
    const BondType type;
    const AtomIdentifier pairId;

public:
    GraphEdge(const AtomIdentifier& pairId, const BondType type) :
        pairId(pairId),
        type(type)
    {}
};