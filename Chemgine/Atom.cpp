#include "Atom.hpp"


Atom::Atom(const std::string& symbol) :
    symbol(symbol)
{}

const Atom Atom::Carbon("C");
const Atom Atom::Oxygen("O");
const Atom Atom::Hydrogen("H");