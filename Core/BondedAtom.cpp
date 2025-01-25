#include "BondedAtom.hpp"

BondedAtom::BondedAtom(std::unique_ptr<const Atom>&& atom, const c_size index) noexcept :
    atom(std::move(atom)),
    index(index)
{}

BondedAtom::BondedAtom(const BondedAtom& other) noexcept :
    index(other.index),
    atom(other.atom->clone()),
    bonds(other.bonds)
{}

bool BondedAtom::isSame(const BondedAtom& other) const
{
    return this->atom.get() == other.atom.get();
}

void BondedAtom::replace(std::unique_ptr<const Atom>&& atom)
{
    this->atom = std::move(atom);
}
