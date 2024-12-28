#pragma once

#include "BoostUnits.hpp"
#include "MetaUtils.hpp"
#include "Precision.hpp"
#include "Printers.hpp"

// --- Boost quantity wrapper ---

using float_q = float_s;

template <typename UnitT>
class Quantity;

template<typename QuantityT>
constexpr inline auto wrapQuantity(const QuantityT quantity) noexcept
{
	static_assert(Utils::is_specialization_of_v<QuantityT, boost::units::quantity>,
		"wrapQuantity: The provided quantity type must be a boost::units::quantity specialization.");

	return Quantity<typename QuantityT::unit_type>(quantity);
};

template <typename UnitT>
// Class for handling compile-time unitized values.
class Quantity : public boost::units::quantity<UnitT, float_q>
{
	static_assert(boost::units::is_unit<UnitT>::value || boost::units::is_dimensionless_unit<UnitT>::value,
		"Quantity: UnitT must be a boost::units::unit specialization.");
public:
	using Base = boost::units::quantity<UnitT, float_q>;

private:
	using Base::from_value; // Hide method

public:
	using Base::Base;

	constexpr Quantity(const Base quantity) noexcept;

	template <typename = std::enable_if_t<boost::units::is_dimensionless_unit<UnitT>::value>>
	constexpr Quantity(const float_q value) noexcept;

	template <typename SrcT>
	static constexpr inline Quantity from(const SrcT quantity) noexcept;
	static constexpr inline Quantity from(const float_q value) noexcept;

	template <typename DstT, typename = std::enable_if_t<!boost::units::is_dimensionless_unit<DstT>::value>>
	constexpr inline Quantity<DstT> to() const noexcept;

	template <typename DstT, typename = std::enable_if_t<
		boost::units::is_dimensionless_unit<DstT>::value&&
		is_dimless_convertible_v<base_unit_of<UnitT>::type>>>
		constexpr inline auto to() const noexcept;

	constexpr inline auto operator+() const noexcept;
	constexpr inline auto operator-() const noexcept;

	template <typename RhsUnitT>
	constexpr inline auto operator+(Quantity<RhsUnitT> rhs) const noexcept;
	template <typename RhsUnitT>
	constexpr inline auto operator-(Quantity<RhsUnitT> rhs) const noexcept;
	template <typename RhsUnitT>
	constexpr inline auto operator*(Quantity<RhsUnitT> rhs) const noexcept;
	template <typename RhsUnitT>
	constexpr inline auto operator/(Quantity<RhsUnitT> rhs) const noexcept;

	const std::string& unitSymbol() const;
	const std::string& unitName() const;

	static const std::string& getUnitSymbol();
	static const std::string& getUnitName();

	std::string toString() const;

	friend std::ostream& operator<<(std::ostream& os, const Quantity quantity)
	{
		return os << quantity.value() << ' ' << quantity.getUnitSymbol();
	}

	using UnitType = UnitT;
	using BaseUnit = base_unit_of<UnitT>::type;
	using StorageType = float_q;
};

template <typename UnitT>
constexpr Quantity<UnitT>::Quantity(const Base quantity) noexcept :
	Base(quantity)
{}

template <typename UnitT>
template <typename>
constexpr Quantity<UnitT>::Quantity(const float_q value) noexcept :
	Base(value, 0)
{}

template <typename UnitT>
template <typename SrcT>
constexpr inline Quantity<UnitT> Quantity<UnitT>::from(const SrcT quantity) noexcept
{
	return static_cast<Quantity>(quantity);
}

template <typename UnitT>
constexpr inline Quantity<UnitT> Quantity<UnitT>::from(const float_q value) noexcept
{
	return Quantity(from_value(value));
}

template <typename UnitT>
template <typename DstT, typename>
constexpr inline Quantity<DstT> Quantity<UnitT>::to() const noexcept
{
	return static_cast<Quantity<DstT>>(*this);
}

template <typename UnitT>
template <typename DstT, typename>
constexpr inline auto Quantity<UnitT>::to() const noexcept
{
	return Quantity<DstT>::from(Base::value());
}

template <typename UnitT>
constexpr inline auto Quantity<UnitT>::operator+() const noexcept
{
	return wrapQuantity(+static_cast<Base>(*this));
}

template <typename UnitT>
constexpr inline auto Quantity<UnitT>::operator-() const noexcept
{
	return wrapQuantity(-static_cast<Base>(*this));
}

template <typename UnitT>
template <typename RhsUnitT>
constexpr inline auto Quantity<UnitT>::operator+(Quantity<RhsUnitT> rhs) const noexcept
{
	return wrapQuantity(static_cast<Base>(*this) + static_cast<Quantity<RhsUnitT>::Base>(rhs));
}

template <typename UnitT>
template <typename RhsUnitT>
constexpr inline auto Quantity<UnitT>::operator-(Quantity<RhsUnitT> rhs) const noexcept
{
	return wrapQuantity(static_cast<Base>(*this) - static_cast<Quantity<RhsUnitT>::Base>(rhs));
}

template <typename UnitT>
template <typename RhsUnitT>
constexpr inline auto Quantity<UnitT>::operator*(Quantity<RhsUnitT> rhs) const noexcept
{
	return wrapQuantity(static_cast<Base>(*this) * static_cast<Quantity<RhsUnitT>::Base>(rhs));
}

template <typename UnitT>
template <typename RhsUnitT>
constexpr inline auto Quantity<UnitT>::operator/(Quantity<RhsUnitT> rhs) const noexcept
{
	return wrapQuantity(static_cast<Base>(*this) / static_cast<Quantity<RhsUnitT>::Base>(rhs));
}

template<typename UnitT, typename FloatT>
constexpr inline auto operator*(const FloatT lhs, const Quantity<UnitT> rhs)
{
	static_assert(std::is_floating_point_v<FloatT>, "FloatT must be a floating point type.");
	return wrapQuantity(lhs * static_cast<Quantity<UnitT>::Base>(rhs));
}

template<typename UnitT, typename FloatT>
constexpr inline auto operator/(const FloatT lhs, const Quantity<UnitT> rhs)
{
	static_assert(std::is_floating_point_v<FloatT>, "FloatT must be a floating point type.");
	return wrapQuantity(lhs / static_cast<Quantity<UnitT>::Base>(rhs));
}

template <typename UnitT>
const std::string& Quantity<UnitT>::getUnitSymbol()
{
	if constexpr (boost::units::is_dimensionless_unit<UnitT>::value)
	{
		const static std::string symbol = "1";
		return symbol;
	}
	else
	{
		const static std::string symbol = symbol_string(UnitT{});
		return symbol;
	}
}

template <typename UnitT>
const std::string& Quantity<UnitT>::getUnitName()
{
	if constexpr (boost::units::is_dimensionless_unit<UnitT>::value)
	{
		const static std::string name = "";
		return name;
	}
	else
	{
		const static std::string name = name_string(UnitT{});
		return name;
	}
}

template <typename UnitT>
const std::string& Quantity<UnitT>::unitSymbol() const
{
	return getUnitSymbol();
}

template <typename UnitT>
const std::string& Quantity<UnitT>::unitName() const
{
	return getUnitName();
}

template <typename UnitT>
std::string Quantity<UnitT>::toString() const
{
	std::stringstream str;
	str << *this;
	return str.str();
}


// --- Printers ---

template <typename UnitT>
class Def::Printer<Quantity<UnitT>>
{
public:
	static std::string print(const Quantity<UnitT> object)
	{
		return Def::print(object.value());
	}

	static std::string prettyPrint(const Quantity<UnitT> object)
	{
		const auto valStr = Def::prettyPrint(object.value());
		if constexpr (boost::units::is_dimensionless_unit<UnitT>::value)
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
		return symbol_string(Unit{}) + " (abs.)";                    \
	}                                                                \
                                                                     \
	inline std::string name_string(const Abs##Unit&)                 \
	{                                                                \
		return "absolute " + name_string(Unit{});                    \
	}                                                                \
                                                                     \
    static constexpr auto _Abs##Unit = Quantity(1.0f * Abs##Unit{}); \

// Defines the conversion factor and offset between two units.
#define CHG_UNIT_CONVERSION(Source, Destination, Factor, Offset)                                                          \
	BOOST_UNITS_DEFINE_CONVERSION_FACTOR(Source##BaseUnit, Destination, float_q, static_cast<float_q>(Factor)); \
	BOOST_UNITS_DEFINE_CONVERSION_OFFSET(Source##BaseUnit, Destination, float_q, static_cast<float_q>(Offset)); \

// Defines the custom symbol and name for a multiply-composite unit.
#define CHG_MUL_UNIT(Lhs, Rhs)                                                      \
	static_assert(!is_divide_composite_unit<Lhs> && !is_divide_composite_unit<Rhs>, \
		"Non-canonical unit: Non-root division operator.");                         \
                                                                                    \
	using Lhs##Rhs = boost::units::multiply_typeof_helper<Lhs, Rhs>::type;          \
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
#define CHG_SCALED_UNIT(Unit, Base, Exp, Prefix)                                  \
	using Prefix##Unit = boost::units::make_scaled_unit<Unit, boost::units::scale<Base, boost::units::static_rational<Exp>>>::type; \


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
