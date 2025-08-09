#pragma once

#include "atomics/kinds/Atom.hpp"
#include "atomics/Bond.hpp"

#include <memory>
#include <vector>

class BondedAtomBase
{
protected:
    BondedAtomBase(
        const c_size index,
        std::vector<Bond>&& bonds
    ) noexcept;

public:
    c_size index;
    std::vector<Bond> bonds;

    BondedAtomBase(const BondedAtomBase&) = default;
    BondedAtomBase(BondedAtomBase&&) = default;
    virtual ~BondedAtomBase() = default;

    BondedAtomBase& operator=(BondedAtomBase&&) = default;

    bool isSame(const BondedAtomBase& other) const;

    virtual const Atom& getAtom() const = 0;
    virtual std::unique_ptr<BondedAtomBase> clone() const = 0;

    const Bond* getBondTo(const BondedAtomBase& other) const;

    std::unique_ptr<BondedAtomBase> mutate(const Atom& atom);

    static std::unique_ptr<BondedAtomBase> create(
        const Symbol& symbol, const c_size index, std::vector<Bond>&& bonds);
    static std::unique_ptr<BondedAtomBase> create(
        const Atom& atom, const c_size index, std::vector<Bond>&& bonds);
};


template<typename AtomT>
class BondedAtom : public BondedAtomBase
{
    static_assert(std::is_base_of_v<Atom, AtomT>,
        "BondedAtom: AtomT must be an Atom derived type.");

private:
    const AtomT atom;

public:
    BondedAtom(
        const AtomT& atom,
        const c_size index,
        std::vector<Bond>&& bonds
    ) noexcept;
    BondedAtom(const BondedAtom&) = default;
    BondedAtom(BondedAtom&&) = default;

    const AtomT& getAtom() const override final;
    std::unique_ptr<BondedAtomBase> clone() const override final;
};

template<typename AtomT>
BondedAtom<AtomT>::BondedAtom(
    const AtomT& atom,
    const c_size index,
    std::vector<Bond>&& bonds
) noexcept :
    BondedAtomBase(index, std::move(bonds)),
    atom(atom)
{}

template<typename AtomT>
const AtomT& BondedAtom<AtomT>::getAtom() const
{
    return atom;
}

template<typename AtomT>
std::unique_ptr<BondedAtomBase> BondedAtom<AtomT>::clone() const
{
    return std::make_unique<BondedAtom<AtomT>>(this->atom, index, utils::copy(this->bonds));
}
