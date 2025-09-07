#include "atomics/BondedAtom.hpp"

#include "atomics/kinds/Radical.hpp"

BondedAtomBase::BondedAtomBase(const c_size index, std::vector<Bond>&& bonds) noexcept :
    index(index),
    bonds(std::move(bonds))
{}

bool BondedAtomBase::isSame(const BondedAtomBase& other) const { return this == &other; }

const Bond* BondedAtomBase::getBondTo(const BondedAtomBase& other) const
{
    // The number of bonds is generally <=8 and likely <=4.
    // Simple O(N) search is a decent approach.
    const auto bondIt = std::ranges::find_if(bonds, [&](const auto& b) { return other.isSame(b.getOther()); });
    return bondIt != bonds.end() ? &*bondIt : nullptr;
}

std::unique_ptr<BondedAtomBase> BondedAtomBase::mutate(const AtomBase& atom)
{
    return create(atom, index, std::move(bonds));
}

std::unique_ptr<BondedAtomBase>
BondedAtomBase::create(const Symbol& symbol, const c_size index, std::vector<Bond>&& bonds)
{
    if (const auto atom = Atom::fromSymbol(symbol)) {
        return static_cast<std::unique_ptr<BondedAtomBase>>(
            std::make_unique<BondedAtom<Atom>>(*atom, index, std::move(bonds)));
    }
    if (const auto radical = Radical::fromSymbol(symbol)) {
        return static_cast<std::unique_ptr<BondedAtomBase>>(
            std::make_unique<BondedAtom<Radical>>(*radical, index, std::move(bonds)));
    }
    return nullptr;
}

std::unique_ptr<BondedAtomBase>
BondedAtomBase::create(const AtomBase& atom, const c_size index, std::vector<Bond>&& bonds)
{
    return atom.isRadical()
               ? static_cast<std::unique_ptr<BondedAtomBase>>(
                     std::make_unique<BondedAtom<Radical>>(static_cast<const Radical&>(atom), index, std::move(bonds)))
               : static_cast<std::unique_ptr<BondedAtomBase>>(
                     std::make_unique<BondedAtom<Atom>>(static_cast<const Atom&>(atom), index, std::move(bonds)));
}
