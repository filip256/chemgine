#include "StateNucleator.hpp"
#include "Reactant.hpp"

StateNucleator::StateNucleator(
	Amount<Unit::CELSIUS>(Reactant::* getTransitionPointCB)() const,
	Amount<Unit::JOULE_PER_MOLE>(Reactant::* getTransitionHeatCB)() const
) noexcept:
	getTransitionPointCB(getTransitionPointCB),
	getTransitionHeatCB(getTransitionHeatCB)
{}

void StateNucleator::setReactant(const Reactant& other)
{
	reactant.emplace(other);
}

void StateNucleator::setTransitionPointCB(Amount<Unit::CELSIUS>(Reactant::* callback)() const)
{
	getTransitionPointCB = callback;
}

void StateNucleator::setTransitionHeatCB(Amount<Unit::JOULE_PER_MOLE>(Reactant::* callback)() const)
{
	getTransitionHeatCB = callback;
}

void StateNucleator::unset()
{
	reactant = std::nullopt;
}

bool StateNucleator::isNull() const
{
	return getTransitionPointCB == nullptr;
}

bool StateNucleator::isSet() const
{
	return reactant.has_value();
}

bool StateNucleator::isValid() const
{
	return isNull() == false && isSet();
}

const Reactant& StateNucleator::getReactant() const
{
	return *reactant;
}

Amount<Unit::CELSIUS> StateNucleator::getTransitionPoint() const
{
	return (*reactant.*getTransitionPointCB)();
}

Amount<Unit::JOULE_PER_MOLE> StateNucleator::getTransitionHeat() const
{
	return (*reactant.*getTransitionHeatCB)();
}

bool StateNucleator::isLower(const Reactant& other) const
{
	return reactant.has_value() ?
		(other.*getTransitionPointCB)() < (*reactant.*getTransitionPointCB)() :
		true;
}

bool StateNucleator::isHigher(const Reactant& other) const
{
	return reactant.has_value() ?
		(other.*getTransitionPointCB)() > (*reactant.*getTransitionPointCB)() :
		true;
}

bool StateNucleator::setIfLower(const Reactant& other)
{
	if (getTransitionPointCB == nullptr)
		return false;

	if (reactant.has_value()  == false ||
		(other.*getTransitionPointCB)() < (*reactant.*getTransitionPointCB)())
	{
		setReactant(other);
		return true;
	}

	return false;
}

bool StateNucleator::setIfHigher(const Reactant& other)
{
	if (getTransitionPointCB == nullptr)
		return false;

	if (reactant.has_value() == false ||
		(other.*getTransitionPointCB)() > (*reactant.*getTransitionPointCB)())
	{
		setReactant(other);
		return true;
	}

	return false;
}