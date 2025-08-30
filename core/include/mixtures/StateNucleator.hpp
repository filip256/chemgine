#pragma once

#include "data/values/Amount.hpp"
#include "reactions/Reactant.hpp"

#include <optional>

class StateNucleator
{
private:
    std::optional<Reactant>      reactant;
    Amount<Unit::CELSIUS>        (Reactant::*getTransitionPointCB)() const = nullptr;
    Amount<Unit::JOULE_PER_MOLE> (Reactant::*getTransitionHeatCB)() const  = nullptr;

public:
    StateNucleator(
        Amount<Unit::CELSIUS>        (Reactant::*getTransitionPointCB)() const,
        Amount<Unit::JOULE_PER_MOLE> (Reactant::*getTransitionHeatCB)() const) noexcept;

    StateNucleator()                      = default;
    StateNucleator(const StateNucleator&) = default;

    void setReactant(const Reactant& other);
    void setTransitionPointCB(Amount<Unit::CELSIUS> (Reactant::*callback)() const);
    void setTransitionHeatCB(Amount<Unit::JOULE_PER_MOLE> (Reactant::*callback)() const);
    void unset();

    bool isNull() const;
    bool isSet() const;
    bool isValid() const;

    const Reactant&              getReactant() const;
    Amount<Unit::CELSIUS>        getTransitionPoint() const;
    Amount<Unit::JOULE_PER_MOLE> getTransitionHeat() const;

    bool isLower(const Reactant& other) const;
    bool isHigher(const Reactant& other) const;

    bool setIfLower(const Reactant& other);
    bool setIfHigher(const Reactant& other);
};
