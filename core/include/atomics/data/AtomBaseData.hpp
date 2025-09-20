#pragma once

#include "data/values/Amount.hpp"
#include "data/values/Symbol.hpp"
#include "structs/ImmutableSet.hpp"

class AtomBaseData
{
public:
    const Symbol                      symbol;
    const std::string                 name;
    const Amount<Unit::GRAM_PER_MOLE> weight;

protected:
    AtomBaseData(Symbol&& symbol, std::string&& name, const Amount<Unit::GRAM_PER_MOLE> weight) noexcept;

public:
    AtomBaseData(const AtomBaseData&) = delete;
    AtomBaseData(AtomBaseData&&)      = default;
    virtual ~AtomBaseData()           = default;

    std::string getSMILES() const;

    virtual bool isRadical() const = 0;

    virtual const ImmutableSet<uint8_t>& getValences() const                          = 0;
    virtual uint8_t                      getFittingValence(const uint8_t bonds) const = 0;
    virtual bool                         hasValence(const uint8_t valence) const      = 0;

    virtual uint8_t getPrecedence() const = 0;

    virtual void dumpDefinition(std::ostream& out, const bool prettify) const = 0;
    void         print(std::ostream& out = std::cout) const;

    static const uint8_t NullValence = utils::npos<uint8_t>;
};
