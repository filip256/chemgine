#pragma once

#include "atomics/data/AtomBaseData.hpp"
#include "data/Accessor.hpp"
#include "structs/Cloneable.hpp"

class AtomBase : public Accessor<>,
                 public CloneableBase<AtomBase>
{
protected:
    const AtomBaseData& data;

    AtomBase(const AtomBaseData& data) noexcept;
    AtomBase(const Symbol& symbol) noexcept;

public:
    virtual ~AtomBase() = default;

    virtual const AtomBaseData& getData() const;

    bool isRadical() const;

    uint8_t       getPrecedence() const;
    const Symbol& getSymbol() const;
    std::string   getSMILES() const;

    bool         equals(const AtomBase& other) const;
    virtual bool matches(const AtomBase& other) const = 0;

    static bool isDefined(const Symbol& symbol);
};
