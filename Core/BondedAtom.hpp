#include "Atom.hpp"
#include "Bond.hpp"
#include "SizeTypedefs.hpp"

#include <memory>
#include <vector>

class BondedAtom
{
private:
    static constexpr c_size NullIndex = static_cast<c_size>(-1);

public:
    c_size index = NullIndex;
    std::unique_ptr<const Atom> atom;
    std::vector<Bond> bonds;

    BondedAtom(
        std::unique_ptr<const Atom>&& atom,
        const c_size index = NullIndex
    ) noexcept;
    BondedAtom(const BondedAtom& other) noexcept;
    BondedAtom(BondedAtom&& other) noexcept = default;

    BondedAtom& operator=(BondedAtom&&) = default;

    bool isSame(const BondedAtom& other) const;

    void replace(std::unique_ptr<const Atom>&& atom);
};
