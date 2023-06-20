#pragma once

#include "BaseComponent.hpp"
#include "AtomData.hpp"

class Atom : public BaseComponent
{
protected:
    AtomIdType id;

public:
    Atom(const AtomIdType id, const ComponentType type = ComponentType::ATOM);

    virtual const AtomData& data() const;
};