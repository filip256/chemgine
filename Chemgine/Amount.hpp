#pragma once

#include "Precision.hpp"
#include "Value.hpp"
#include "Unit.hpp"
#include "NumericUtils.hpp"
#include "Printers.hpp"

#include <string>
#include <limits>
#include <cmath>
#include <numbers>

/// <summary>
/// Stores the amount of a certain Unit and statically handles conversions between units.
/// If a conversion between two units is used but not defined, a compilation error will occur
/// </summary>
template<Unit UnitT = Unit::NONE>
class Amount : public Value<float_s>
{
public:
	using StorageType = float_s;

	constexpr inline Amount() = default;
	constexpr inline Amount(const StorageType value) noexcept;
	constexpr inline Amount(const Amount<UnitT>&) = default;

	template<Unit OUnitT>
	constexpr inline Amount(const Amount<OUnitT>&) noexcept = delete;

	constexpr Amount<UnitT> operator+(const Amount<UnitT> other) const noexcept;
	constexpr Amount<UnitT> operator-(const Amount<UnitT> other) const noexcept;
	constexpr Amount<UnitT> operator*(const Amount<UnitT> other) const noexcept;
	constexpr Amount<UnitT> operator/(const Amount<UnitT> other) const noexcept;
	constexpr Amount<UnitT> operator-() const noexcept;

	constexpr Amount<UnitT> operator+=(const Amount<UnitT> other) noexcept;
	constexpr Amount<UnitT> operator-=(const Amount<UnitT> other) noexcept;

	constexpr inline bool equals(const Amount<UnitT> other,
		const StorageType epsilon = std::numeric_limits<StorageType>::epsilon()) const noexcept;

	constexpr inline StorageType asKilo() const noexcept;
	constexpr inline StorageType asStd() const noexcept;
	constexpr inline StorageType asMilli() const noexcept;
	constexpr inline StorageType asMicro() const noexcept;

	template<Unit OUnitT, Unit FactT1>
	constexpr inline Amount<OUnitT> to(const Amount<FactT1>) const noexcept = delete;

	constexpr inline Unit unit() const noexcept;
	static inline std::string unitSymbol() noexcept = delete;
	static inline std::string unitName() noexcept = delete;
	
	inline std::string toString(const uint8_t maxDigits = 255) const noexcept;
	inline std::string format() noexcept = delete;

	static const Amount<UnitT> Unknown;
	static const Amount<UnitT> Infinity;
	static const Amount<UnitT> Minimum;
	static const Amount<UnitT> Maximum;
	static const Amount<UnitT> Epsilon;

	constexpr inline bool isUnknown() const noexcept;
	constexpr inline bool isInfinity() const noexcept;

	constexpr bool oveflowsOnAdd(const Amount<UnitT> other) const noexcept;
	constexpr bool oveflowsOnMultiply(const Amount<UnitT> other) const noexcept;
};


template<Unit UnitT>
const Amount<UnitT> Amount<UnitT>::Unknown = std::numeric_limits<StorageType>::min() - std::numeric_limits<StorageType>::epsilon();
template<Unit UnitT>
const Amount<UnitT> Amount<UnitT>::Infinity = std::numeric_limits<StorageType>::infinity();
template<Unit UnitT>
const Amount<UnitT> Amount<UnitT>::Minimum = std::numeric_limits<StorageType>::min();
template<Unit UnitT>
const Amount<UnitT> Amount<UnitT>::Maximum = std::numeric_limits<StorageType>::max();
template<Unit UnitT>
const Amount<UnitT> Amount<UnitT>::Epsilon = std::numeric_limits<StorageType>::epsilon();

template<Unit UnitT>
constexpr bool Amount<UnitT>::isUnknown() const noexcept
{
	return this->value == Amount<UnitT>::Unknown.asStd();
}

template<Unit UnitT>
constexpr bool Amount<UnitT>::isInfinity() const noexcept
{
	return this->value == Amount<UnitT>::Infinity.asStd();
}

template<Unit UnitT>
constexpr bool Amount<UnitT>::oveflowsOnAdd(const Amount<UnitT> other) const noexcept
{
	return Value<StorageType>::oveflowsOnAdd(other);
}

template<Unit UnitT>
constexpr bool Amount<UnitT>::oveflowsOnMultiply(const Amount<UnitT> other) const noexcept
{
	return Value<StorageType>::oveflowsOnMultiply(other);
}

template<Unit UnitT>
constexpr Amount<UnitT>::Amount(const StorageType value) noexcept :
	Value<StorageType>(value)
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

template<Unit UnitT>
constexpr Amount<UnitT> Amount<UnitT>::operator-() const noexcept
{
	return Amount<UnitT>(-1 * this->value);
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

template <Unit UnitT>
constexpr inline bool Amount<UnitT>::equals(const Amount<UnitT> other, const StorageType epsilon) const noexcept
{
	return Utils::floatEqual(this->value, other.value, epsilon);
}

template<Unit UnitT>
constexpr Amount<>::StorageType Amount<UnitT>::asStd() const noexcept { return value; }

template<Unit UnitT>
constexpr Amount<>::StorageType Amount<UnitT>::asKilo() const noexcept { return static_cast<StorageType>(value / 1000.0); }
template<>
constexpr Amount<>::StorageType Amount<Unit::CUBIC_METER>::asKilo() const noexcept { return static_cast<StorageType>(value / 1000000000.0); }

template<Unit UnitT>
constexpr Amount<>::StorageType Amount<UnitT>::asMilli() const noexcept { return static_cast<StorageType>(value * 1000.0); }
template<>
constexpr Amount<>::StorageType Amount<Unit::CUBIC_METER>::asMilli() const noexcept { return static_cast<StorageType>(value * 1000000000.0); }

template<Unit UnitT>
constexpr Amount<>::StorageType Amount<UnitT>::asMicro() const noexcept { return static_cast<StorageType>(value * 1000000.0); }
template<>
constexpr Amount<>::StorageType Amount<Unit::CUBIC_METER>::asMicro() const noexcept { return static_cast<StorageType>(value * 1000000000000000000.0); }

template<Unit UnitT>
constexpr Unit Amount<UnitT>::unit() const noexcept { return UnitT; }

template<Unit UnitT>
std::string Amount<UnitT>::toString(const uint8_t maxDigits) const noexcept 
{
	const auto str = Linguistics::formatFloatingPoint(value, maxDigits);
	if constexpr (UnitT != Unit::NONE)
		return str + ' ' + Amount<UnitT>::unitSymbol();
	else
		return str + ' '; // don't print NONE's unit
}

template<>
inline std::string Amount<Unit::NONE>::unitSymbol() noexcept { return "1"; }
template<>
inline std::string Amount<Unit::ANY>::unitSymbol() noexcept { return "*"; }
template<>
inline std::string Amount<Unit::GRAM>::unitSymbol() noexcept { return "g"; }
template<>
inline std::string Amount<Unit::LITER>::unitSymbol() noexcept { return "L"; }
template<>
inline std::string Amount<Unit::CUBIC_METER>::unitSymbol() noexcept { return "m3"; }
template<>
inline std::string Amount<Unit::DROP>::unitSymbol() noexcept { return "drops"; }
template<>
inline std::string Amount<Unit::MOLE>::unitSymbol() noexcept { return "mol"; }
template<>
inline std::string Amount<Unit::SECOND>::unitSymbol() noexcept { return "s"; }
template<>
inline std::string Amount<Unit::CELSIUS>::unitSymbol() noexcept { return "C"; }
template<>
inline std::string Amount<Unit::KELVIN>::unitSymbol() noexcept { return "K"; }
template<>
inline std::string Amount<Unit::FAHRENHEIT>::unitSymbol() noexcept { return "F"; }
template<>
inline std::string Amount<Unit::TORR>::unitSymbol() noexcept { return "torr"; }
template<>
inline std::string Amount<Unit::PASCAL>::unitSymbol() noexcept { return "Pa"; }
template<>
inline std::string Amount<Unit::ATMOSPHERE>::unitSymbol() noexcept { return "atm"; }
template<>
inline std::string Amount<Unit::JOULE>::unitSymbol() noexcept { return "J"; }
template<>
inline std::string Amount<Unit::WATT>::unitSymbol() noexcept { return "W"; }
template<>
inline std::string Amount<Unit::METER>::unitSymbol() noexcept { return "m"; }
template<>
inline std::string Amount<Unit::DEGREE>::unitSymbol() noexcept { return "o"; }
template<>
inline std::string Amount<Unit::RADIAN>::unitSymbol() noexcept { return "rad"; }
template<>
inline std::string Amount<Unit::PER_SECOND>::unitSymbol() noexcept { return "/" + Amount<Unit::SECOND>::unitSymbol(); }
template<>
inline std::string Amount<Unit::PER_METER>::unitSymbol() noexcept { return "/" + Amount<Unit::METER>::unitSymbol(); }
template<>
inline std::string Amount<Unit::MOLE_RATIO>::unitSymbol() noexcept { return Amount<Unit::MOLE>::unitSymbol() + "/" + Amount<Unit::MOLE>::unitSymbol(); }
template<>
inline std::string Amount<Unit::MOLE_PERCENT>::unitSymbol() noexcept { return Amount<Unit::MOLE>::unitSymbol() + "%"; }
template<>
inline std::string Amount<Unit::MOLE_PER_SECOND>::unitSymbol() noexcept { return Amount<Unit::MOLE>::unitSymbol() + "/" + Amount<Unit::SECOND>::unitSymbol(); }
template<>
inline std::string Amount<Unit::GRAM_PER_MOLE>::unitSymbol() noexcept { return Amount<Unit::GRAM>::unitSymbol() + "/" + Amount<Unit::MOLE>::unitSymbol(); }
template<>
inline std::string Amount<Unit::GRAM_PER_MILLILITER>::unitSymbol() noexcept { return Amount<Unit::GRAM>::unitSymbol() + "/m" + Amount<Unit::LITER>::unitSymbol(); }
template<>
inline std::string Amount<Unit::JOULE_PER_MOLE>::unitSymbol() noexcept { return Amount<Unit::JOULE>::unitSymbol() + "/" + Amount<Unit::MOLE>::unitSymbol(); }
template<>
inline std::string Amount<Unit::JOULE_PER_CELSIUS>::unitSymbol() noexcept { return Amount<Unit::JOULE>::unitSymbol() + "/" + Amount<Unit::CELSIUS>::unitSymbol(); }
template<>
inline std::string Amount<Unit::JOULE_PER_MOLE_CELSIUS>::unitSymbol() noexcept { return Amount<Unit::JOULE>::unitSymbol() + "/(" + Amount<Unit::MOLE>::unitSymbol() + "*" + Amount<Unit::CELSIUS>::unitSymbol() + ")"; }
template<>
inline std::string Amount<Unit::TORR_MOLE_RATIO>::unitSymbol() noexcept { return Amount<Unit::TORR>::unitSymbol() + "*(" + Amount<Unit::MOLE>::unitSymbol() + "/" + Amount<Unit::MOLE>::unitSymbol() + ")"; }

template<>
inline std::string Amount<Unit::NONE>::unitName() noexcept { return ""; }
template<>
inline std::string Amount<Unit::ANY>::unitName() noexcept { return "any unit"; }
template<>
inline std::string Amount<Unit::GRAM>::unitName() noexcept { return "gram"; }
template<>
inline std::string Amount<Unit::LITER>::unitName() noexcept { return "liter"; }
template<>
inline std::string Amount<Unit::CUBIC_METER>::unitName() noexcept { return "cubic meter"; }
template<>
inline std::string Amount<Unit::DROP>::unitName() noexcept { return "drops"; }
template<>
inline std::string Amount<Unit::MOLE>::unitName() noexcept { return "mole"; }
template<>
inline std::string Amount<Unit::SECOND>::unitName() noexcept { return "second"; }
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
inline std::string Amount<Unit::ATMOSPHERE>::unitName() noexcept { return "atmosphere"; }
template<>
inline std::string Amount<Unit::JOULE>::unitName() noexcept { return "joule"; }
template<>
inline std::string Amount<Unit::WATT>::unitName() noexcept { return "watt"; }
template<>
inline std::string Amount<Unit::METER>::unitName() noexcept { return "meter"; }
template<>
inline std::string Amount<Unit::DEGREE>::unitName() noexcept { return "degree"; }
template<>
inline std::string Amount<Unit::RADIAN>::unitName() noexcept { return "radian"; }
template<>
inline std::string Amount<Unit::PER_SECOND>::unitName() noexcept { return "/" + Amount<Unit::SECOND>::unitName(); }
template<>
inline std::string Amount<Unit::PER_METER>::unitName() noexcept { return "/" + Amount<Unit::METER>::unitName(); }
template<>
inline std::string Amount<Unit::MOLE_RATIO>::unitName() noexcept { return Amount<Unit::MOLE>::unitName() + " / " + Amount<Unit::MOLE>::unitName(); }
template<>
inline std::string Amount<Unit::MOLE_PERCENT>::unitName() noexcept { return Amount<Unit::MOLE>::unitName() + " %"; }
template<>
inline std::string Amount<Unit::MOLE_PER_SECOND>::unitName() noexcept { return Amount<Unit::MOLE>::unitName() + " / " + Amount<Unit::SECOND>::unitName(); }
template<>
inline std::string Amount<Unit::GRAM_PER_MOLE>::unitName() noexcept { return Amount<Unit::GRAM>::unitName() + " / " + Amount<Unit::MOLE>::unitName(); }
template<>
inline std::string Amount<Unit::GRAM_PER_MILLILITER>::unitName() noexcept { return Amount<Unit::GRAM>::unitName() + " / milli" + Amount<Unit::LITER>::unitName(); }
template<>
inline std::string Amount<Unit::JOULE_PER_MOLE>::unitName() noexcept { return Amount<Unit::JOULE>::unitName() + " / " + Amount<Unit::MOLE>::unitName(); }
template<>
inline std::string Amount<Unit::JOULE_PER_CELSIUS>::unitName() noexcept { return Amount<Unit::JOULE>::unitName() + " / " + Amount<Unit::CELSIUS>::unitName(); }
template<>
inline std::string Amount<Unit::JOULE_PER_MOLE_CELSIUS>::unitName() noexcept { return Amount<Unit::JOULE>::unitName() + " / (" + Amount<Unit::MOLE>::unitName() + " + " + Amount<Unit::CELSIUS>::unitName() + ")"; }
template<>
inline std::string Amount<Unit::TORR_MOLE_RATIO>::unitName() noexcept { return Amount<Unit::TORR>::unitName() + " * (" + Amount<Unit::MOLE>::unitName() + " / " + Amount<Unit::MOLE>::unitName() + ")"; }

template<>
inline std::string Amount<Unit::SECOND>::format() noexcept { return Linguistics::formatTime(static_cast<int32_t>(asMilli())); }

//    --- Direct Conversions ---    //

template<>
template<>
constexpr Amount<Unit::LITER>::Amount(const Amount<Unit::CUBIC_METER>& cubicMeters) noexcept :
	Value(static_cast<StorageType>(cubicMeters.asStd() * 1000.0))
{}

template<>
template<>
constexpr Amount<Unit::LITER>::Amount(const Amount<Unit::DROP>& drops) noexcept :
	Value(static_cast<StorageType>(drops.asStd() / 20000.0))
{}

template<>
template<>
constexpr Amount<Unit::CUBIC_METER>::Amount(const Amount<Unit::LITER>& liters) noexcept :
	Value(static_cast<StorageType>(liters.asStd() / 1000.0))
{}

template<>
template<>
constexpr Amount<Unit::CUBIC_METER>::Amount(const Amount<Unit::DROP>& drops) noexcept :
	Amount<Unit::CUBIC_METER>(Amount<Unit::LITER>(drops))
{}

template<>
template<>
constexpr Amount<Unit::DROP>::Amount(const Amount<Unit::LITER>& liters) noexcept :
	Value(static_cast<StorageType>(liters.asStd() * 20000.0))
{}

template<>
template<>
constexpr Amount<Unit::DROP>::Amount(const Amount<Unit::CUBIC_METER>& cubicMeters) noexcept :
	Amount<Unit::DROP>(Amount<Unit::LITER>(cubicMeters))
{}

template<>
template<>
constexpr Amount<Unit::JOULE_PER_MOLE>::Amount(const Amount<Unit::JOULE>& joules) noexcept :
	Value(static_cast<StorageType>(joules.asStd()))
{}

template<>
template<>
constexpr Amount<Unit::KELVIN>::Amount(const Amount<Unit::CELSIUS>& c) noexcept :
	Value(static_cast<StorageType>(c.asStd() + 273.15))
{}

template<>
template<>
constexpr Amount<Unit::CELSIUS>::Amount(const Amount<Unit::KELVIN>& k) noexcept :
	Value(static_cast<StorageType>(k.asStd() - 273.15))
{}

template<>
template<>
constexpr Amount<Unit::FAHRENHEIT>::Amount(const Amount<Unit::CELSIUS>& c) noexcept :
	Value(static_cast<StorageType>(c.asStd() * (9.0 / 5.0) + 32))
{}

template<>
template<>
constexpr Amount<Unit::CELSIUS>::Amount(const Amount<Unit::FAHRENHEIT>& f) noexcept :
	Value(static_cast<StorageType>(f.asStd() * (5.0 / 9.0) - 32))
{}

template<>
template<>
constexpr Amount<Unit::FAHRENHEIT>::Amount(const Amount<Unit::KELVIN>& k) noexcept :
	Amount<Unit::FAHRENHEIT>(Amount<Unit::CELSIUS>(k))
{}

template<>
template<>
constexpr Amount<Unit::KELVIN>::Amount(const Amount<Unit::FAHRENHEIT>& f) noexcept :
	Amount<Unit::KELVIN>(Amount<Unit::CELSIUS>(f))
{}

template<>
template<>
constexpr Amount<Unit::TORR>::Amount(const Amount<Unit::PASCAL>& p) noexcept :
	Value(static_cast<StorageType>(p.asStd() / 133.3223684211))
{}

template<>
template<>
constexpr Amount<Unit::TORR>::Amount(const Amount<Unit::ATMOSPHERE>& a) noexcept :
	Value(static_cast<StorageType>(a.asStd() * 760.0))
{}

template<>
template<>
constexpr Amount<Unit::PASCAL>::Amount(const Amount<Unit::TORR>& t) noexcept :
	Value(static_cast<StorageType>(t.asStd() * 133.3223684211))
{}

template<>
template<>
constexpr Amount<Unit::PASCAL>::Amount(const Amount<Unit::ATMOSPHERE>& a) noexcept :
	Amount<Unit::PASCAL>(Amount<Unit::TORR>(a))
{}

template<>
template<>
constexpr Amount<Unit::ATMOSPHERE>::Amount(const Amount<Unit::TORR>& t) noexcept :
	Value(static_cast<StorageType>(t.asStd() / 760.0))
{}

template<>
template<>
constexpr Amount<Unit::ATMOSPHERE>::Amount(const Amount<Unit::PASCAL>& p) noexcept :
	Amount<Unit::ATMOSPHERE>(Amount<Unit::TORR>(p))
{}

template<>
template<>
constexpr Amount<Unit::DEGREE>::Amount(const Amount<Unit::RADIAN>& radians) noexcept :
	Amount<Unit::DEGREE>(static_cast<StorageType>(radians.asStd() * (180.0 / std::numbers::pi_v<StorageType>)))
{}

template<>
template<>
constexpr Amount<Unit::RADIAN>::Amount(const Amount<Unit::DEGREE>& degrees) noexcept :
	Amount<Unit::RADIAN>(static_cast<StorageType>(degrees.asStd() * (std::numbers::pi_v<StorageType> / 180.0)))
{}

template<>
template<>
constexpr Amount<Unit::MOLE_RATIO>::Amount(const Amount<Unit::MOLE_PERCENT>& ratio) noexcept :
	Amount<Unit::MOLE_RATIO>(static_cast<StorageType>(ratio.asStd() / 100.0))
{}

template<>
template<>
constexpr Amount<Unit::MOLE_PERCENT>::Amount(const Amount<Unit::MOLE_RATIO>& ratio) noexcept :
	Amount<Unit::MOLE_PERCENT>(static_cast<StorageType>(ratio.asStd() * 100.0))
{}

//    --- Indirect Conversions ---    //

template<>
template<>
constexpr Amount<Unit::NONE> Amount<Unit::PER_SECOND>::to(const Amount<Unit::SECOND> timespan) const noexcept
{
	return value * timespan.asStd();
}

template<>
template<>
constexpr Amount<Unit::NONE> Amount<Unit::PER_METER>::to(const Amount<Unit::METER> distance) const noexcept
{
	return value * distance.asStd();
}

template<>
template<>
constexpr Amount<Unit::LITER> Amount<Unit::GRAM>::to(const Amount<Unit::GRAM_PER_MILLILITER> density) const noexcept
{
	return static_cast<StorageType>((value / density.asStd()) / 1000.0);
}

template<>
template<>
constexpr Amount<Unit::GRAM> Amount<Unit::LITER>::to(const Amount<Unit::GRAM_PER_MILLILITER> density) const noexcept
{
	return static_cast<StorageType>((value * density.asStd()) * 1000.0);
}

template<>
template<>
constexpr Amount<Unit::GRAM_PER_MILLILITER> Amount<Unit::LITER>::to(const Amount<Unit::GRAM> grams) const noexcept
{
	return grams.asStd() / asMilli();
}

template<>
template<>
constexpr Amount<Unit::GRAM_PER_MILLILITER> Amount<Unit::GRAM>::to(const Amount<Unit::LITER> liters) const noexcept
{
	return value / liters.asMilli();
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
constexpr Amount<Unit::MOLE_RATIO> Amount<Unit::TORR>::to(const Amount<Unit::TORR_MOLE_RATIO> henry) const noexcept
{
	return value / henry.asStd();
}

template<>
template<>
constexpr Amount<Unit::MOLE> Amount<Unit::MOLE_RATIO>::to(const Amount<Unit::MOLE> moles) const noexcept
{
	return value * moles.asStd();
}

template<>
template<>
constexpr Amount<Unit::MOLE_RATIO> Amount<Unit::MOLE>::to(const Amount<Unit::MOLE> moles) const noexcept
{
	return value / moles.asStd();
}

template<>
template<>
constexpr Amount<Unit::GRAM_PER_MOLE> Amount<Unit::GRAM>::to(const Amount<Unit::MOLE> moles) const noexcept
{
	return value / moles.asStd();
}

template<>
template<>
constexpr Amount<Unit::GRAM> Amount<Unit::GRAM_PER_MOLE>::to(const Amount<Unit::MOLE> moles) const noexcept
{
	return value * moles.asStd();
}

template<>
template<>
constexpr Amount<Unit::MOLE> Amount<Unit::GRAM_PER_MOLE>::to(const Amount<Unit::GRAM> grams) const noexcept
{
	return grams.asStd() / value;
}

template<>
template<>
constexpr Amount<Unit::JOULE> Amount<Unit::JOULE_PER_MOLE>::to(const Amount<Unit::MOLE> moles) const noexcept
{
	return value * moles.asStd();
}

template<>
template<>
constexpr Amount<Unit::MOLE> Amount<Unit::JOULE_PER_MOLE>::to(const Amount<Unit::JOULE> joules) const noexcept
{
	return joules.asStd() / value;
}

template<>
template<>
constexpr Amount<Unit::JOULE> Amount<Unit::JOULE_PER_CELSIUS>::to(const Amount<Unit::CELSIUS> temperature) const noexcept
{
	return value * temperature.asStd();
}

template<>
template<>
constexpr Amount<Unit::CELSIUS> Amount<Unit::JOULE_PER_CELSIUS>::to(const Amount<Unit::JOULE> joules) const noexcept
{
	return joules.asStd() / value;
}

template<>
template<>
constexpr Amount<Unit::JOULE> Amount<Unit::WATT>::to(const Amount<Unit::SECOND> seconds) const noexcept
{
	return value * seconds.asStd();
}

template<>
template<>
constexpr Amount<Unit::SECOND> Amount<Unit::WATT>::to(const Amount<Unit::JOULE> joules) const noexcept
{
	return joules.asStd() / value;
}

template<>
template<>
constexpr Amount<Unit::WATT> Amount<Unit::JOULE>::to(const Amount<Unit::SECOND> seconds) const noexcept
{
	return value / seconds.asStd();
}

template<>
template<>
constexpr Amount<Unit::WATT> Amount<Unit::SECOND>::to(const Amount<Unit::JOULE> joules) const noexcept
{
	return value * joules.asStd();
}

template<>
template<>
constexpr Amount<Unit::JOULE_PER_MOLE> Amount<Unit::JOULE_PER_MOLE_CELSIUS>::to(const Amount<Unit::CELSIUS> temperature) const noexcept
{
	return value * temperature.asStd();
}

template<>
template<>
constexpr Amount<Unit::JOULE_PER_MOLE_CELSIUS> Amount<Unit::JOULE_PER_MOLE>::to(const Amount<Unit::CELSIUS> temperature) const noexcept
{
	return value / temperature.asStd();
}

template<>
template<>
constexpr Amount<Unit::JOULE_PER_CELSIUS> Amount<Unit::JOULE_PER_MOLE_CELSIUS>::to(const Amount<Unit::MOLE> moles) const noexcept
{
	return value * moles.asStd();
}

template<>
template<>
constexpr Amount<Unit::JOULE_PER_MOLE_CELSIUS> Amount<Unit::JOULE_PER_CELSIUS>::to(const Amount<Unit::MOLE> moles) const noexcept
{
	return value / moles.asStd();
}

template<>
template<>
constexpr Amount<Unit::CELSIUS> Amount<Unit::JOULE>::to(const Amount<Unit::JOULE_PER_CELSIUS> heatCapacity) const noexcept
{
	return value / heatCapacity.asStd();
}

template<>
template<>
constexpr Amount<Unit::MOLE> Amount<Unit::JOULE>::to(const Amount<Unit::JOULE_PER_MOLE> latentHeat) const noexcept
{
	return value / latentHeat.asStd();
}

template<>
template<>
constexpr Amount<Unit::MOLE> Amount<Unit::MOLE_PER_SECOND>::to(const Amount<Unit::SECOND> timespan) const noexcept
{
	return value * timespan.asStd();
}


//    --- Literals ---    //

constexpr inline Amount<Unit::LITER> operator""_L(long double value) { return static_cast<Amount<>::StorageType>(value); }
constexpr inline Amount<Unit::CUBIC_METER> operator""_m3(long double value) { return static_cast<Amount<>::StorageType>(value); }
constexpr inline Amount<Unit::GRAM> operator""_g(long double value) { return static_cast<Amount<>::StorageType>(value); }
constexpr inline Amount<Unit::MOLE> operator""_mol(long double value) { return static_cast<Amount<>::StorageType>(value); }
constexpr inline Amount<Unit::SECOND> operator""_s(long double value) { return static_cast<Amount<>::StorageType>(value); }
constexpr inline Amount<Unit::CELSIUS> operator""_C(long double value) { return static_cast<Amount<>::StorageType>(value); }
constexpr inline Amount<Unit::KELVIN> operator""_K(long double value) { return static_cast<Amount<>::StorageType>(value); }
constexpr inline Amount<Unit::FAHRENHEIT> operator""_F(long double value) { return static_cast<Amount<>::StorageType>(value); }
constexpr inline Amount<Unit::TORR> operator""_torr(long double value) { return static_cast<Amount<>::StorageType>(value); }
constexpr inline Amount<Unit::PASCAL> operator""_Pa(long double value) { return static_cast<Amount<>::StorageType>(value); }
constexpr inline Amount<Unit::ATMOSPHERE> operator""_atm(long double value) { return static_cast<Amount<>::StorageType>(value); }
constexpr inline Amount<Unit::JOULE> operator""_J(long double value) { return static_cast<Amount<>::StorageType>(value); }
constexpr inline Amount<Unit::WATT> operator""_W(long double value) { return static_cast<Amount<>::StorageType>(value); }
constexpr inline Amount<Unit::METER> operator""_m(long double value) { return static_cast<Amount<>::StorageType>(value); }
constexpr inline Amount<Unit::DEGREE> operator""_o(long double value) { return static_cast<Amount<>::StorageType>(value); }
constexpr inline Amount<Unit::RADIAN> operator""_rad(long double value) { return static_cast<Amount<>::StorageType>(value); }

//    --- Printer ---    //

template <Unit U>
class Def::Printer<Amount<U>>
{
public:
	static std::string print(const Amount<U> object)
	{
		return Def::print(object.asStd());
	}

	static std::string prettyPrint(const Amount<U> object)
	{
		const auto valStr = Def::prettyPrint(object.asStd());
		if constexpr (U != Unit::NONE)
			return valStr + '_' + object.unitSymbol();
		else
			return valStr;
	}
};
