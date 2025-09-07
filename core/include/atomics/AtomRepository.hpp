#pragma once

#include "atomics/data/AtomData.hpp"
#include "atomics/data/RadicalData.hpp"
#include "data/def/Object.hpp"

class AtomRepository
{
private:
    using ContainerT = std::unordered_map<Symbol, std::unique_ptr<AtomBaseData>>;
    ContainerT atoms;

public:
    AtomRepository()                      = default;
    AtomRepository(const AtomRepository&) = delete;
    AtomRepository(AtomRepository&&)      = default;

    template <typename AtomT>
    bool add(const def::Object& definition);

    const AtomBaseData* find(const Symbol& symbol) const;
    bool                contains(const Symbol& symbol) const;
    const AtomBaseData& at(const Symbol& symbol) const;

    size_t totalDefinitionCount() const;

    using Iterator = ContainerT::const_iterator;
    Iterator begin() const;
    Iterator end() const;

    void clear();

    static constexpr size_t npos = static_cast<size_t>(-1);
};
