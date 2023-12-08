#pragma once

#include <string>

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
	constexpr inline Amount() = default;
	constexpr inline Amount(const double value) noexcept;
	constexpr inline Amount(const Value<double> value) noexcept;
	constexpr inline Amount(const Amount<UnitT>&) = default;

	template<Unit OUnitT>
	constexpr inline Amount(const Amount<OUnitT>&) noexcept = delete;
	template<Unit OUnitT1, Unit OUnitT2>
	constexpr inline Amount(const Amount<OUnitT1>&, const Amount<OUnitT2>&) noexcept = delete;

	constexpr Amount<UnitT> operator+(const Amount<UnitT> other) const noexcept;
	constexpr Amount<UnitT> operator-(const Amount<UnitT> other) const noexcept;
	constexpr Amount<UnitT> operator*(const Amount<UnitT> other) const noexcept;
	constexpr Amount<UnitT> operator/(const Amount<UnitT> other) const noexcept;

	constexpr Amount<UnitT> operator+=(const Amount<UnitT> other) noexcept;
	constexpr Amount<UnitT> operator-=(const Amount<UnitT> other) noexcept;

	constexpr inline double asKilo() const noexcept;
	constexpr inline double asStd() const noexcept;
	constexpr inline double asMilli() const noexcept;
	constexpr inline double asMicro() const noexcept;

	template<Unit OUnitT, Unit FactT1>
	constexpr inline Amount<OUnitT> to(const Amount<FactT1>) const noexcept = delete;
	template<Unit OUnitT, Unit FactT1, Unit FactT2>
	constexpr inline Amount<OUnitT> to(const Amount<FactT1>, const Amount<FactT2>) const noexcept = delete;

	constexpr inline Unit unit() const noexcept;
	static inline std::string unitName() noexcept = delete;
};


template<Unit UnitT>
constexpr Amount<UnitT>::Amount(const double value) noexcept :
	Value<double>(value)
{}

template<Unit UnitT>
constexpr Amount<UnitT>::Amount(const Value<double> value) noexcept :
	Value<double>(value) 
{}

template<Unit UnitT>
constexpr Amount<UnitT> Amount<UnitT>::operator+(const Amount<UnitT> other) const noexcept
{
	return Amount<UnitT>(this->value + other.value);
}

template<Unit UnitT>
constexpr Amount<UnitT> Amount<UnitT>::operator-(const Amount<UnitT> other) const noexcept 
{
	return Amount<UnitT>(this->value - other.value);
}

template<Unit UnitT>
constexpr Amount<UnitT> Amount<UnitT>::operator*(const Amount<UnitT> other) const noexcept
{
	return Amount<UnitT>(this->value * other.value);
}

template<Unit UnitT>
constexpr Amount<UnitT> Amount<UnitT>::operator/(const Amount<UnitT> other) const noexcept
{
	return Amount<UnitT>(this->value / other.value);
}

template <Unit UnitT>
constexpr Amount<UnitT> Amount<UnitT>::operator+=(const Amount<UnitT> other) noexcept
{
	value += other.value;
	return *this;
}

template <Unit UnitT>
constexpr Amount<UnitT> Amount<UnitT>::operator-=(const Amount<UnitT> other) noexcept
{
	value -= other.value;
	return *this;
}

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


template<>
inline std::string Amount<Unit::UNIT>::unitName() noexcept { return "unit"; }
template<>
inline std::string Amount<Unit::GRAM>::unitName() noexcept { return "gram"; }
template<>
inline std::string Amount<Unit::LITER>::unitName() noexcept { return "liter"; }
template<>
inline std::string Amount<Unit::MOLE>::unitName() noexcept { return "mole"; }
template<>
inline std::string Amount<Unit::SECOND>::unitName() noexcept { return "second"; }
template<>
inline std::string Amount<Unit::CUBIC_METER>::unitName() noexcept { return "cubic meter"; }
template<>
inline std::string Amount<Unit::CELSIUS>::unitName() noexcept { return "celsius"; }
template<>
inline std::string Amount<Unit::KELVIN>::unitName() noexcept { return "kelvin"; }
template<>
inline std::string Amount<Unit::FAHRENHEIT>::unitName() noexcept { return "fahrenheit"; }
template<>
inline std::string Amount<Unit::TORR>::unitName() noexcept { return "torr"; }
template<>
inline std::string Amount<Unit::PASCAL>::unitName() noexcept { return "pascal"; }
template<>
inline std::string Amount<Unit::MOLE_PER_SECOND>::unitName() noexcept { return Amount<Unit::MOLE>::unitName() + " per " + Amount<Unit::SECOND>::unitName(); }


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
constexpr Amount<Unit::GRAM_PER_LITER>::Amount(const Amount<Unit::GRAM>& grams, const Amount<Unit::LITER>& liters) noexcept :
	Value<double>(grams.asStd() / liters.asMilli())
{}

template<>
template<>
constexpr Amount<Unit::KELVIN>::Amount(const Amount<Unit::CELSIUS>& c) noexcept :
	Value<double>(c.asStd() - 273.15)
{}

template<>
template<>
constexpr Amount<Unit::CELSIUS>::Amount(const Amount<Unit::KELVIN>& k) noexcept :
	Value<double>(k.asStd() + 273.15)
{}

template<>
template<>
constexpr Amount<Unit::FAHRENHEIT>::Amount(const Amount<Unit::CELSIUS>& c) noexcept :
	Value<double>(c.asStd() * (9.0 / 5.0) + 32)
{}

template<>
template<>
constexpr Amount<Unit::CELSIUS>::Amount(const Amount<Unit::FAHRENHEIT>& f) noexcept :
	Value<double>(f.asStd() * (5.0 / 9.0) - 32)
{}

template<>
template<>
constexpr Amount<Unit::FAHRENHEIT>::Amount(const Amount<Unit::KELVIN>& k) noexcept :
	Value<double>(Amount<Unit::CELSIUS>(k))
{}

template<>
template<>
constexpr Amount<Unit::KELVIN>::Amount(const Amount<Unit::FAHRENHEIT>& f) noexcept :
	Value<double>(Amount<Unit::CELSIUS>(f))
{}


template<>
template<>
constexpr Amount<Unit::PASCAL>::Amount(const Amount<Unit::TORR>& t) noexcept :
	Value<double>(t.asStd() * 133.3223684211)
{}

template<>
template<>
constexpr Amount<Unit::TORR>::Amount(const Amount<Unit::PASCAL>& p) noexcept :
	Value<double>(p.asStd() / 133.3223684211)
{}



template<>
template<>
constexpr Amount<Unit::LITER> Amount<Unit::GRAM>::to(const Amount<Unit::GRAM_PER_LITER> density) const noexcept
{
	return value * density.asStd();
}
template<>
template<>
constexpr Amount<Unit::GRAM> Amount<Unit::LITER>::to(const Amount<Unit::GRAM_PER_LITER> density) const noexcept
{
	return value / density.asStd();
}


template<>
template<>
constexpr Amount<Unit::GRAM> Amount<Unit::MOLE>::to(const Amount<Unit::GRAM_PER_MOLE> molarMass) const noexcept
{
	return value * molarMass.asStd();
}
template<>
template<>
constexpr Amount<Unit::MOLE> Amount<Unit::GRAM>::to(const Amount<Unit::GRAM_PER_MOLE> molarMass) const noexcept
{
	return value / molarMass.asStd();
}






template<>
template<>
constexpr Amount<Unit::LITER> Amount<Unit::MOLE>::to(const Amount<Unit::GRAM_PER_MOLE> molarMass, const Amount<Unit::GRAM_PER_LITER> density) const noexcept
{
	return this->to<Unit::GRAM>(molarMass).to<Unit::LITER>(density);
}
template<>
template<>
constexpr Amount<Unit::MOLE> Amount<Unit::LITER>::to(const Amount<Unit::GRAM_PER_MOLE> molarMass, const Amount<Unit::GRAM_PER_LITER> density) const noexcept
{
	return this->to<Unit::GRAM>(density).to<Unit::MOLE>(molarMass);
}