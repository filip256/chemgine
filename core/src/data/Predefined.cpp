#include "data/Predefined.hpp"

#include "data/DataStore.hpp"

Predefined::Predefined() noexcept :
    Hydrogen(*Atom::fromSymbol("H")),
    Carbon(*Atom::fromSymbol("C"))
{}

const Predefined& Predefined::get()
{
    static const Predefined predefined;
    return predefined;
}
