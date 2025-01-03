#pragma once

#include "BoostUnits.hpp"
#include "MetaUtils.hpp"
#include "Precision.hpp"
#include "Printers.hpp"

// --- Boost quantity wrapper ---

using float_q = float_s;

template <typename UnitT>
concept UnitType = boost::units::is_unit<UnitT>::value;


template <UnitType U>
class Quantity;

template<typename QuantityT>
constexpr inline auto wrapQuantity(const QuantityT quantity) noexcept
{
	static_assert(Utils::is_specialization_of_v<QuantityT, boost::units::quantity>,
		"wrapQuantity: The provided quantity type must be a boost::units::quantity specialization.");

	return Quantity<typename QuantityT::unit_type>(quantity);
};


template <UnitType U>
// Class for handling compile-time unitized values.
class Quantity : public boost::units::quantity<U, float_q>
{
public:
	using Base = boost::units::quantity<U, float_q>;

private:
	using Base::from_value; // Hide method

public:
	using Base::Base;

	constexpr Quantity(const Base quantity) noexcept;

	template <typename = std::enable_if_t<boost::units::is_dimensionless_unit<U>::value>>
	constexpr Quantity(const float_q value) noexcept;

	template <typename SrcT>
	static constexpr inline Quantity from(const SrcT quantity) noexcept;
	static constexpr inline Quantity from(const float_q value) noexcept;

	template <UnitType DstU, typename = std::enable_if_t<!boost::units::is_dimensionless_unit<DstU>::value>>
	constexpr inline Quantity<DstU> to() const noexcept;

	template <UnitType DstU, typename = std::enable_if_t<
		boost::units::is_dimensionless_unit<DstU>::value&&
		is_dimless_convertible_v<base_unit_of<U>::type>>>
		constexpr inline auto to() const noexcept;

	constexpr inline auto operator+() const noexcept;
	constexpr inline auto operator-() const noexcept;

	template <UnitType RhsU>
	constexpr inline auto operator+(const Quantity<RhsU> rhs) const noexcept;
	template <UnitType RhsU>
	constexpr inline auto operator-(const Quantity<RhsU> rhs) const noexcept;
	template <UnitType RhsU>
	constexpr inline auto operator*(const Quantity<RhsU> rhs) const noexcept;
	template <UnitType RhsU>
	constexpr inline auto operator/(const Quantity<RhsU> rhs) const noexcept;

	template <typename FloatT>
	constexpr inline Quantity operator+(const FloatT rhs) const noexcept;
	template <typename FloatT>
	constexpr inline Quantity operator-(const FloatT rhs) const noexcept;
	template <typename FloatT>
	constexpr inline Quantity operator*(const FloatT rhs) const noexcept;
	template <typename FloatT>
	constexpr inline Quantity operator/(const FloatT rhs) const noexcept;

	template <UnitType OthU>
	constexpr inline bool equals(const Quantity<OthU> other,
		const float_q epsilon = std::numeric_limits<StorageType>::epsilon()) const noexcept;

	const std::string& unitSymbol() const;
	const std::string& unitName() const;

	static const std::string& getUnitSymbol();
	static const std::string& getUnitName();

	std::string toString() const;

	bool oveflowsOnAdd(const Quantity other) const noexcept;
	bool oveflowsOnMultiply(const Quantity other) const noexcept;

	friend std::ostream& operator<<(std::ostream& os, const Quantity quantity)
	{
		return os << quantity.value() << ' ' << quantity.getUnitSymbol();
	}

	using UnitType = U;
	using BaseUnit = base_unit_of<U>::type;
	using StorageType = float_q;
};

template <UnitType U>
constexpr Quantity<U>::Quantity(const Base quantity) noexcept :
	Base(quantity)
{}

template <UnitType U>
template <typename>
constexpr Quantity<U>::Quantity(const float_q value) noexcept :
	Base(value, 0)
{}

template <UnitType U>
template <typename SrcT>
constexpr inline Quantity<U> Quantity<U>::from(const SrcT quantity) noexcept
{
	return static_cast<Quantity>(quantity);
}

template <UnitType U>
constexpr inline Quantity<U> Quantity<U>::from(const float_q value) noexcept
{
	return Quantity(from_value(value));
}

template <UnitType U>
template <UnitType DstU, typename>
constexpr inline Quantity<DstU> Quantity<U>::to() const noexcept
{
	return static_cast<Quantity<DstU>>(*this);
}

template <UnitType U>
template <UnitType DstU, typename>
constexpr inline auto Quantity<U>::to() const noexcept
{
	return Quantity<DstU>::from(Base::value());
}

template <UnitType U>
constexpr inline auto Quantity<U>::operator+() const noexcept
{
	return wrapQuantity(+static_cast<Base>(*this));
}

template <UnitType U>
constexpr inline auto Quantity<U>::operator-() const noexcept
{
	return wrapQuantity(-static_cast<Base>(*this));
}

template <UnitType U>
template <UnitType RhsU>
constexpr inline auto Quantity<U>::operator+(const Quantity<RhsU> rhs) const noexcept
{
	return wrapQuantity(static_cast<Base>(*this) + static_cast<Quantity<RhsU>::Base>(rhs));
}

template <UnitType U>
template <UnitType RhsU>
constexpr inline auto Quantity<U>::operator-(const Quantity<RhsU> rhs) const noexcept
{
	return wrapQuantity(static_cast<Base>(*this) - static_cast<Quantity<RhsU>::Base>(rhs));
}

template <UnitType U>
template <UnitType RhsU>
constexpr inline auto Quantity<U>::operator*(const Quantity<RhsU> rhs) const noexcept
{
	return wrapQuantity(static_cast<Base>(*this) * static_cast<Quantity<RhsU>::Base>(rhs));
}

template <UnitType U>
template <UnitType RhsU>
constexpr inline auto Quantity<U>::operator/(const Quantity<RhsU> rhs) const noexcept
{
	return wrapQuantity(static_cast<Base>(*this) / static_cast<Quantity<RhsU>::Base>(rhs));
}

template <UnitType U>
template <typename FloatT>
constexpr inline Quantity<U> Quantity<U>::operator+(const FloatT rhs) const noexcept
{
	static_assert(std::is_floating_point_v<FloatT>, "FloatT must be a floating point type.");
	return Quantity::from(this->value() + rhs);
}

template <UnitType U>
template <typename FloatT>
constexpr inline Quantity<U> Quantity<U>::operator-(const FloatT rhs) const noexcept
{
	static_assert(std::is_floating_point_v<FloatT>, "FloatT must be a floating point type.");
	return Quantity::from(this->value() - rhs);
}

template <UnitType U>
template <typename FloatT>
constexpr inline Quantity<U> Quantity<U>::operator*(const FloatT rhs) const noexcept
{
	static_assert(std::is_floating_point_v<FloatT>, "FloatT must be a floating point type.");
	return Quantity::from(this->value() * rhs);
}

template <UnitType U>
template <typename FloatT>
constexpr inline Quantity<U> Quantity<U>::operator/(const FloatT rhs) const noexcept
{
	static_assert(std::is_floating_point_v<FloatT>, "FloatT must be a floating point type.");
	return Quantity::from(this->value() / rhs);
}

template<UnitType U, typename FloatT>
constexpr inline auto operator*(const FloatT lhs, const Quantity<U> rhs)
{
	static_assert(std::is_floating_point_v<FloatT>, "FloatT must be a floating point type.");
	return wrapQuantity(lhs * static_cast<Quantity<U>::Base>(rhs));
}

template<UnitType U, typename FloatT>
constexpr inline auto operator/(const FloatT lhs, const Quantity<U> rhs)
{
	static_assert(std::is_floating_point_v<FloatT>, "FloatT must be a floating point type.");
	return wrapQuantity(lhs / static_cast<Quantity<U>::Base>(rhs));
}

template <UnitType U>
template <UnitType OthU>
constexpr inline bool Quantity<U>::equals(const Quantity<OthU> other, const float_q epsilon) const noexcept
{
	return Utils::floatEqual(this->value(), other.value(), epsilon);
}

template <UnitType U>
const std::string& Quantity<U>::getUnitSymbol()
{
	if constexpr (boost::units::is_dimensionless_unit<U>::value)
	{
		const static std::string symbol = "1";
		return symbol;
	}
	else
	{
		const static std::string symbol = symbol_string(U{});
		return symbol;
	}
}

template <UnitType U>
const std::string& Quantity<U>::getUnitName()
{
	if constexpr (boost::units::is_dimensionless_unit<U>::value)
	{
		const static std::string name = "";
		return name;
	}
	else
	{
		const static std::string name = name_string(U{});
		return name;
	}
}

template <UnitType U>
const std::string& Quantity<U>::unitSymbol() const
{
	return getUnitSymbol();
}

template <UnitType U>
const std::string& Quantity<U>::unitName() const
{
	return getUnitName();
}

template <UnitType U>
std::string Quantity<U>::toString() const
{
	std::stringstream str;
	str << *this;
	return str.str();
}

template<UnitType U>
bool Quantity<U>::oveflowsOnAdd(const Quantity other) const noexcept
{
	const auto thisVal = this->value();
	const auto otherVal = other.value();

	return
		thisVal > 0.0 && otherVal > 0.0 ?
			thisVal > std::numeric_limits<float_q>::max() - otherVal :
		thisVal < 0.0 && otherVal < 0.0 ?
			thisVal < std::numeric_limits<float_q>::min() - otherVal :
		false;
}

template<UnitType U>
bool Quantity<U>::oveflowsOnMultiply(const Quantity other) const noexcept
{
	const auto thisVal = this->Base::value();
	const auto otherVal = other.value();

	return
		thisVal > 0.0 ?
			(
				otherVal > 0.0 ?
					thisVal > std::numeric_limits<float_q>::max() / otherVal :
					thisVal > std::numeric_limits<float_q>::min() / otherVal) :
			(
				otherVal > 0.0 ?
					thisVal < std::numeric_limits<float_q>::min() / otherVal :
					thisVal < std::numeric_limits<float_q>::max() / otherVal
			);
}


// --- Printers ---

template <UnitType U>
class Def::Printer<Quantity<U>>
{
public:
	static std::string print(const Quantity<U> object)
	{
		return Def::print(object.value());
	}

	static std::string prettyPrint(const Quantity<U> object)
	{
		const auto valStr = Def::prettyPrint(object.value());
		if constexpr (boost::units::is_dimensionless_unit<U>::value)
			return valStr + '_' + object.unitSymbol();
		else
			return valStr;
	}
};



// --- Macros ---

// Defines a boost::units::unit for the base_unit with the corresponding name.
#define _CHG_UNIT_TYPE(Name)                                 \
    using Name = Name##BaseUnit::unit_type;                  \
                                                             \
    template<>                                               \
    struct base_unit_of<Name>                                \
    {                                                        \
        using type = Name##BaseUnit;                         \
    };                                                       \
                                                             \
    static constexpr auto _##Name = Quantity(1.0f * Name{}); \

// Defines a boost::units::base_unit with the given symbol and name.
#define CHG_UNIT(Name, Symbol, Dimension)                                                   \
	struct Name##BaseUnit : boost::units::base_unit<Name##BaseUnit, Dimension, __COUNTER__> \
	{                                                                                       \
		static const char* symbol() { return Symbol; }                                      \
		static const char* name() { return #Name; }                                         \
	};                                                                                      \
	_CHG_UNIT_TYPE(Name)                                                                    \


// Defines ratio and percent quasi-dimensionless units derived from a given base unit.
#define CHG_RATIO_UNIT(Name)                                                                           \
	using Name##RatioBaseUnit = ratio_base_unit<Name##BaseUnit, __COUNTER__, __COUNTER__>;             \
    using Name##PercentBaseUnit = percent_base_unit<Name##BaseUnit, __COUNTER__, __COUNTER__>;         \
	BOOST_UNITS_DEFINE_CONVERSION_FACTOR(Name##RatioBaseUnit, Name##PercentBaseUnit, float_q, 100.0f); \
    _CHG_UNIT_TYPE(Name##Ratio)                                                                        \
	_CHG_UNIT_TYPE(Name##Percent)                                                                      \

// Defines an absolute unit derived from a given relative unit.
#define CHG_ABSOLUTE_UNIT(Unit)                                      \
    using Abs##Unit = boost::units::absolute<Unit>;                  \
                                                                     \
    template<>                                                       \
    struct base_unit_of<Abs##Unit>                                   \
    {                                                                \
        using type = base_unit_of<Unit>::type;                       \
    };                                                               \
                                                                     \
	inline std::string symbol_string(const Abs##Unit&)               \
	{                                                                \
		return symbol_string(Unit{}) + "_abs";                       \
	}                                                                \
                                                                     \
	inline std::string name_string(const Abs##Unit&)                 \
	{                                                                \
		return "absolute " + name_string(Unit{});                    \
	}                                                                \
                                                                     \
    static constexpr auto _Abs##Unit = Quantity(1.0f * Abs##Unit{}); \

// Defines the conversion factor and offset between two units.
#define CHG_UNIT_CONVERSION(Source, Destination, Factor, Offset)                                                \
	BOOST_UNITS_DEFINE_CONVERSION_FACTOR(Source##BaseUnit, Destination, float_q, static_cast<float_q>(Factor)); \
	BOOST_UNITS_DEFINE_CONVERSION_OFFSET(Source##BaseUnit, Destination, float_q, static_cast<float_q>(Offset)); \

// Defines the custom symbol and name for a multiply-composite unit.
#define CHG_MUL_UNIT(Lhs, Rhs)                                                      \
	static_assert(!is_divide_composite_unit<Lhs> && !is_divide_composite_unit<Rhs>, \
		"Non-canonical unit: Non-root division operator.");                         \
                                                                                    \
	using Lhs##Rhs = boost::units::multiply_typeof_helper<Lhs, Rhs>::type;          \
    static constexpr auto _##Lhs##Rhs = Quantity(1.0f * Lhs##Rhs{});                \
                                                                                    \
	template<>                                                                      \
	struct composite_unit_trait<Lhs##Rhs>                                           \
	{                                                                               \
		static constexpr bool MultiplyComposite = true;                             \
		static constexpr bool DivideComposite =                                     \
			composite_unit_trait<Lhs>::DivideComposite ||                           \
            composite_unit_trait<Rhs>::DivideComposite;                             \
	};                                                                              \
																		            \
	inline std::string symbol_string(const Lhs##Rhs&)                               \
	{                                                                               \
		return symbol_string(Lhs{}) + '*' + symbol_string(Rhs{});                   \
	}                                                                               \
                                                                                    \
	inline std::string name_string(const Lhs##Rhs&)                                 \
	{                                                                               \
		return name_string(Lhs{}) + name_string(Rhs{});                             \
	}                                                                               \

// Defines the custom symbol and name for a divide-composite unit.
#define CHG_DIV_UNIT(Lhs, Rhs)                                                          \
		static_assert(!is_divide_composite_unit<Lhs> && !is_divide_composite_unit<Rhs>, \
			"Non-canonical unit: More than one division.");                             \
                                                                                        \
	using Lhs##Per##Rhs = boost::units::divide_typeof_helper<Lhs, Rhs>::type;           \
    static constexpr auto _##Lhs##Per##Rhs = Quantity(1.0f * Lhs##Per##Rhs{});          \
																			            \
	template<>                                                                          \
	struct composite_unit_trait<Lhs##Per##Rhs>                                          \
	{                                                                                   \
		static constexpr bool MultiplyComposite =                                       \
			composite_unit_trait<Lhs>::MultiplyComposite ||                             \
            composite_unit_trait<Rhs>::MultiplyComposite;                               \
		static constexpr bool DivideComposite = true;                                   \
	};                                                                                  \
                                                                                        \
	inline std::string symbol_string(const Lhs##Per##Rhs&)                              \
	{                                                                                   \
		if constexpr (is_composite_unit<Lhs>)                                           \
		{                                                                               \
			if constexpr (is_composite_unit<Rhs>)                                       \
				return '(' + symbol_string(Lhs{}) + ")/(" + symbol_string(Rhs{}) + ')'; \
			else                                                                        \
				return '(' + symbol_string(Lhs{}) + ")/" + symbol_string(Rhs{});        \
		}                                                                               \
		else if constexpr (is_composite_unit<Rhs>)                                      \
			return symbol_string(Lhs{}) + "/(" + symbol_string(Rhs{}) + ')';            \
		else                                                                            \
			return symbol_string(Lhs{}) + '/' + symbol_string(Rhs{});                   \
	}                                                                                   \
                                                                                        \
	inline std::string name_string(const Lhs##Per##Rhs&)                                \
	{                                                                                   \
		return name_string(Lhs{}) + " per " + name_string(Rhs{});                       \
	}                                                                                   \


// Defines the custom symbol and name for an inverse unit.
#define CHG_INV_UNIT(Unit)                                                                \
		static_assert(!is_divide_composite_unit<Unit>,                                    \
			"Non-canonical unit: Inversion of a divide-composite unit.");                 \
                                                                                          \
	using Per##Unit = boost::units::divide_typeof_helper<float_q, Unit>::type::unit_type; \
	static constexpr auto _##Per##Unit = Quantity(1.0f * Per##Unit{});                    \
																			              \
	template<>                                                                            \
	struct composite_unit_trait<Per##Unit>                                                \
	{                                                                                     \
		static constexpr bool MultiplyComposite =                                         \
			composite_unit_trait<Unit>::MultiplyComposite;                                \
		static constexpr bool DivideComposite = true;                                     \
	};                                                                                    \
                                                                                          \
	inline std::string symbol_string(const Per##Unit&)                                    \
	{                                                                                     \
		if constexpr (is_composite_unit<Unit>)                                            \
			return '(' + symbol_string(Unit{}) + ")^-1";                                  \
		else                                                                              \
			return symbol_string(Unit{}) + "^-1";                                         \
	}                                                                                     \
                                                                                          \
	inline std::string name_string(const Per##Unit&)                                      \
	{                                                                                     \
		return "per " + name_string(Unit{});                                              \
	}                                                                                     \

// Creates a scaled unit from another unit.
#define CHG_SCALED_UNIT(Unit, Base, Exp, Prefix)                                    \
	using Prefix##Unit = boost::units::make_scaled_unit<                            \
        Unit, boost::units::scale<Base, boost::units::static_rational<Exp>>>::type; \
                                                                                    \
    static constexpr auto _##Prefix##Unit = Quantity(1.0f * Prefix##Unit{});        \


// Defines a boost::units::homogeneous_system and the dimensionless unit for that system.
#define CHG_UNIT_SYSTEM(Name, ...)                                  \
	using Name = boost::units::make_system<__VA_ARGS__>::type;      \
	using Dimless = boost::units::dimensionless_unit<Name>::type;   \
                                                                    \
	template<>                                                      \
	struct base_unit_of<Dimless>                                    \
	{                                                               \
		using type = int;                                           \
	};                                                              \
                                                                    \
	static constexpr auto _Dimless = Quantity<Dimless>::from(1.0f); \
