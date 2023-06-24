#pragma once

#include "BaseComponent.hpp"
#include "AtomData.hpp"

class Atom : public BaseComponent
{
protected:
    AtomIdType id;

public:
    Atom(const AtomIdType id, const ComponentType type = ComponentType::ATOM);

    Atom(const std::string& symbol);
    Atom(const char symbol);

    virtual const AtomData& data() const;

    static bool isDefined(const AtomIdType id);
    static bool isDefined(const std::string& symbol);
    static bool isDefined(const char symbol);
};