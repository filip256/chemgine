#pragma once

#include "AtomicComponent.hpp"
#include "AtomData.hpp"


class Atom : public AtomicComponent
{
private:
    Atom(const Atom&) = default;

public:
    Atom(const ComponentIdType id);
    Atom(const std::string& symbol);
    Atom(const char symbol);
    Atom(Atom&& atom) noexcept = default;
    ~Atom() noexcept = default;

    const AtomData& data() const override final;

    bool isRadicalType() const override final;

    static bool isDefined(const ComponentIdType id);
    static bool isDefined(const std::string& symbol);
    static bool isDefined(const char symbol);

    uint8_t getPrecedence() const override final;

    std::unordered_map<ComponentIdType, c_size> getComponentCountMap() const override final;

    Atom* clone() const override final;
};
