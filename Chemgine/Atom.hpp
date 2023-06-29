#pragma once

#include "BaseComponent.hpp"
#include "AtomData.hpp"

class Atom : public BaseComponent
{
protected:
    ComponentIdType id;

public:
    Atom(const ComponentIdType id, const ComponentType type = ComponentType::ATOM);

    Atom(const std::string& symbol);
    Atom(const char symbol);

    virtual const AtomData& data() const final;

    static bool isDefined(const ComponentIdType id);
    static bool isDefined(const std::string& symbol);
    static bool isDefined(const char symbol);
};