#pragma once

#include "Component.hpp"

#include <string>

class Atom : public Component
{
    std::string symbol;

public:
    Atom(const std::string& symbol);

    static const Atom Carbon;
    static const Atom Oxygen;
    static const Atom Hydrogen;
};