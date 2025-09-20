#pragma once

#include "atomics/kinds/Atom.hpp"

class Predefined
{
public:
    const Atom Hydrogen;
    const Atom Carbon;

private:
    Predefined() noexcept;

public:
    static const Predefined& get();
};
