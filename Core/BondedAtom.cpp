#include "BondedAtom.hpp"
#include "Radical.hpp"

BondedAtomBase::BondedAtomBase(
    const c_size index,
    std::vector<Bond>&& bonds
) noexcept :
    index(index),
    bonds(std::move(bonds))
{}

bool BondedAtomBase::isSame(const BondedAtomBase& other) const
{
    return this == &other;
}

const Bond* BondedAtomBase::getBondTo(const BondedAtomBase& other) const
{
    // The number of bonds is generally <=8 and likely <=4.
    // Simple O(N) search is a decent approach.
    const auto bondIt = std::ranges::find_if(bonds, [&](const auto& b) { return other.isSame(b.getOther()); });
    return bondIt != bonds.end() ? &*bondIt : nullptr;
}

std::unique_ptr<BondedAtomBase> BondedAtomBase::mutate(const Atom& atom)
{
    return create(atom, index, std::move(bonds));
}

std::unique_ptr<BondedAtomBase> BondedAtomBase::create(
    const Symbol& symbol, const c_size index, std::vector<Bond>&& bonds)
{
    return Radical::isDefined(symbol) ?
        static_cast<std::unique_ptr<BondedAtomBase>>(std::make_unique<BondedAtom<Radical>>(Radical(symbol), index, std::move(bonds))) :
        static_cast<std::unique_ptr<BondedAtomBase>>(std::make_unique<BondedAtom<Atom>>(Atom(symbol), index, std::move(bonds)));
}

std::unique_ptr<BondedAtomBase> BondedAtomBase::create(
    const Atom& atom, const c_size index, std::vector<Bond>&& bonds)
{
    return atom.isRadical() ?
        static_cast<std::unique_ptr<BondedAtomBase>>(std::make_unique<BondedAtom<Radical>>(static_cast<const Radical&>(atom), index, std::move(bonds))) :
        static_cast<std::unique_ptr<BondedAtomBase>>(std::make_unique<BondedAtom<Atom>>(atom, index, std::move(bonds)));
}
