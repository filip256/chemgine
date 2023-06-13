#pragma once

#include "BaseComponent.hpp"
#include "AtomData.hpp"

class Atom : public BaseComponent
{
    AtomIdType id;

public:
    Atom(const AtomIdType id);
};