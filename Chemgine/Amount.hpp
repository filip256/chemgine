#pragma once

#include <string>
#include <cassert>

#include "Value.hpp"
#include "Unit.hpp"

/// <summary>
/// Stores the amount of a certain Unit and statically handles conversions between units.
/// If a conversion between two units is used but not defined, a compilation error will occur
/// </summary>
template<Unit UnitT>
class Amount : public Value<double>
{
public:
	constexpr inline Amount(const double value) noexcept;
	constexpr inline Amount(const Amount<UnitT>&) = default;

	template<Unit OUnitT>
	constexpr inline Amount(const Amount<OUnitT>&) noexcept = delete;
	template<Unit OUnitT1, Unit OUnitT2>
	constexpr inline Amount(const Amount<OUnitT1>&, const Amount<OUnitT2>&) noexcept = delete;

	constexpr inline double asKilo() const noexcept;
	constexpr inline double asStd() const noexcept;
	constexpr inline double asMilli() const noexcept;
	constexpr inline double asMicro() const noexcept;

	// undefined conversions will fail to compile
	template<Unit OUnitT>
	constexpr inline Amount<OUnitT> to(const double) const noexcept = delete;
	template<Unit OUnitT>
	constexpr inline Amount<OUnitT> to(const double, const double) const noexcept = delete;

	constexpr inline Unit unit() const noexcept;
	std::string unitName() const noexcept;

	static std::string nameOf(const Unit unit) noexcept;
};

template<Unit UnitT>
constexpr Amount<UnitT>::Amount(const double value) noexcept :
	Value<double>(value){}

template<Unit UnitT>
constexpr double Amount<UnitT>::asStd() const noexcept { return value; }

template<Unit UnitT>
constexpr double Amount<UnitT>::asKilo() const noexcept { return value / 1000.0; }
template<>
constexpr double Amount<Unit::CUBIC_METER>::asKilo() const noexcept { return value / 1000000000.0; }

template<Unit UnitT>
constexpr double Amount<UnitT>::asMilli() const noexcept { return value * 1000.0; }
template<>
constexpr double Amount<Unit::CUBIC_METER>::asMilli() const noexcept { return value * 1000000000.0; }

template<Unit UnitT>
constexpr double Amount<UnitT>::asMicro() const noexcept { return value * 1000000.0; }
template<>
constexpr double Amount<Unit::CUBIC_METER>::asMicro() const noexcept { return value * 1000000000000000000.0; }

template<Unit UnitT>
constexpr Unit Amount<UnitT>::unit() const noexcept { return UnitT; }

template<Unit UnitT>
std::string Amount<UnitT>::unitName() const noexcept { return nameOf(UnitT); }

template<Unit UnitT>
std::string Amount<UnitT>::nameOf(const Unit unit) noexcept 
{
	switch (unit)
	{
	case Unit::UNIT:
		return "unit";
	case Unit::GRAM:
		return "gram";
	case Unit::LITER:
		return "liter";
	case Unit::MOLE:
		return "mole";
	case Unit::CUBIC_METER:
		return "cubic meter";
	case Unit::OF_DENSITY:
		return nameOf(Unit::GRAM) + "/milli" + nameOf(Unit::LITER);
	default:
		return "";
	}
}

template<>
template<>
constexpr Amount<Unit::LITER>::Amount(const Amount<Unit::CUBIC_METER>& cubicMeters) noexcept :
	Value<double>(cubicMeters.asStd() * 1000.0)
{}

template<>
template<>
constexpr Amount<Unit::CUBIC_METER>::Amount(const Amount<Unit::LITER>& liters) noexcept :
	Value<double>(liters.asStd() / 1000.0)
{}

template<>
template<>
constexpr Amount<Unit::OF_DENSITY>::Amount(const Amount<Unit::GRAM>& grams, const Amount<Unit::LITER>& liters) noexcept :
	Value<double>(grams.asStd() / liters.asMilli())
{}




template<>
template<>
constexpr Amount<Unit::LITER> Amount<Unit::GRAM>::to(const double density) const noexcept
{
	return value * density;
}
template<>
template<>
constexpr Amount<Unit::GRAM> Amount<Unit::LITER>::to(const double density) const noexcept
{
	return value / density;
}


template<>
template<>
constexpr Amount<Unit::GRAM> Amount<Unit::MOLE>::to(const double molarMass) const noexcept
{
	return value * molarMass;
}
template<>
template<>
constexpr Amount<Unit::MOLE> Amount<Unit::GRAM>::to(const double molarMass) const noexcept
{
	return value / molarMass;
}

template<>
template<>
constexpr Amount<Unit::LITER> Amount<Unit::MOLE>::to(const double molarMass, const double density) const noexcept
{
	return this->to<Unit::GRAM>(molarMass).to<Unit::LITER>(density);
}
template<>
template<>
constexpr Amount<Unit::MOLE> Amount<Unit::LITER>::to(const double molarMass, const double density) const noexcept
{
	return this->to<Unit::GRAM>(density).to<Unit::MOLE>(molarMass);
}