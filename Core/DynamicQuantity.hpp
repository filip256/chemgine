#pragma once

#include "TypeId.hpp"
#include "Quantity.hpp"
#include "Parsers.hpp"

#include <tuple>
#include <optional>
#include <unordered_map>

struct AnyUnit {};

class UnitId : public TypeId
{
public:
	using TypeId::TypeId;

	template<typename UnitT>
	static UnitId of();
};

template<typename UnitT>
UnitId UnitId::of()
{
	static_assert(boost::units::is_unit<UnitT>::value || std::is_same_v<UnitT, AnyUnit>,
		"UnitId: UnitT must be a unit type.");
	return UnitId(typeid(UnitT));
}

template<>
struct std::hash<UnitId> : public std::hash<TypeId> {};


// Class for handling run-time unitized values.
class DynamicQuantity
{
private:
	float_q val;
	UnitId unit;

	using Converter = DynamicQuantity(DynamicQuantity::*)() const;
	using ConverterMap = std::unordered_map<UnitId, DynamicQuantity::Converter>;

	template<UnitType SrcU, UnitType DstU>
	DynamicQuantity convert() const;
	template<UnitType DstU>
	inline static const ConverterMap& getConvertersTo();

	template<UnitType U>
	static const std::string& symbolOf();
	template<UnitType U>
	static const std::string& nameOf();

	using Converters = const ConverterMap& (*)();
	using Symbol = Utils::unique_func_t<const std::string& (*)(), 0>;
	using Name = Utils::unique_func_t<const std::string& (*)(), 1>;
	using UnitInfo = std::tuple<Converters, Symbol, Name>;

	template<UnitType U>
	static constexpr inline std::pair<UnitId, UnitInfo> makeUnitInfo();

	template<typename InfoT>
	static const auto& getUnitInfo(const UnitId unit);
	static const UnitInfo& getUnitInfo(const UnitId unit);

public:
	DynamicQuantity(const float_q value, const UnitId unit) noexcept;
	DynamicQuantity(const DynamicQuantity&) = default;

	template<UnitType U>
	DynamicQuantity(const Quantity<U> quantity) noexcept;

	float_q value() const;
	UnitId getUnit() const;

	template<UnitType DstU>
	std::optional<Quantity<DstU>> to() const;
	std::optional<DynamicQuantity> to(const UnitId target) const;

	const std::string& unitSymbol() const;
	const std::string& unitName() const;

	static const std::string& getUnitSymbol(const UnitId unit);
	static const std::string& getUnitName(const UnitId unit);

	static std::optional<UnitId> parseUnitSymbol(const std::string& symbol);

	template <UnitType U>
	static std::optional<Quantity<U>> parse(const float_q value, const std::string& symbol);
	static std::optional<DynamicQuantity> parse(const float_q value, const std::string& symbol);

	static const UnitId AnyUnitId;

	friend std::ostream& operator<<(std::ostream& os, const DynamicQuantity& quantity);
};

template<UnitType U>
DynamicQuantity::DynamicQuantity(const Quantity<U> quantity) noexcept :
	val(quantity.value()),
	unit(UnitId::of<U>())
{}

template<UnitType DstU>
std::optional<Quantity<DstU>> DynamicQuantity::to() const
{
	if (unit == UnitId::of<DstU>() || unit == AnyUnitId)
		return Quantity<DstU>::from(val);

	const auto& converters = getConvertersTo<DstU>();

	const auto it = converters.find(unit);
	return it != converters.end() ?
		std::optional(Quantity<DstU>::from((this->*(it->second))().val)) :
		std::nullopt;
}

template<UnitType SrcU, UnitType DstU>
DynamicQuantity DynamicQuantity::convert() const
{
	return Quantity<SrcU>::from(val).to<DstU>();
}

template<UnitType SrcU>
inline const DynamicQuantity::ConverterMap& DynamicQuantity::getConvertersTo()
{
	static const ConverterMap empty{};
	return empty;
}

template<UnitType U>
const std::string& DynamicQuantity::symbolOf()
{
	return Quantity<U>::getUnitSymbol();
}

template<UnitType U>
const std::string& DynamicQuantity::nameOf()
{
	return Quantity<U>::getUnitName();
}

template<UnitType U>
constexpr inline std::pair<UnitId, DynamicQuantity::UnitInfo> DynamicQuantity::makeUnitInfo()
{
	return std::pair(UnitId::of<U>(), std::tuple(getConvertersTo<U>, symbolOf<U>, nameOf<U>));
}

template<typename InfoT>
const auto& DynamicQuantity::getUnitInfo(const UnitId unit)
{
	return std::get<InfoT>(getUnitInfo(unit))();
}

template <UnitType U>
std::optional<Quantity<U>> DynamicQuantity::parse(const float_q value, const std::string& symbol)
{
	const auto dynQuantity = DynamicQuantity::parse(value, symbol);
	return dynQuantity ?
		std::optional(dynQuantity->to<U>()) :
		std::nullopt;
}

// --- Parsers ---

template <>
class Def::Parser<UnitId>
{
public:
	static std::optional<UnitId> parse(const std::string& str)
	{
		return DynamicQuantity::parseUnitSymbol(Utils::strip(str));
	}
};


template <UnitType U>
class Def::Parser<Quantity<U>>
{
public:
	static std::optional<Quantity<U>> parse(const std::string& str)
	{
		const auto pair = Utils::split(Utils::strip(str), '_', true);
		if (pair.empty())
			return std::nullopt;

		const auto val = Def::parse<float_q>(Utils::strip(pair.front()));
		if (not val)
			return std::nullopt;

		if (pair.size() == 1)
			return Quantity<U>::from(*val);

		if (pair.size() == 2)
			return DynamicQuantity::parse<U>(*val, pair.back());

		return std::nullopt;
	}
};


template <>
class Def::Parser<DynamicQuantity>
{
public:
	static std::optional<DynamicQuantity> parse(const std::string& str)
	{
		const auto pair = Utils::split(Utils::strip(str), '_', true);
		if (pair.empty())
			return std::nullopt;

		const auto val = Def::parse<float_q>(pair.front());
		if (not val)
			return std::nullopt;

		if (pair.size() == 1)
			return DynamicQuantity(*val, DynamicQuantity::AnyUnitId);

		if (pair.size() == 2)
			return DynamicQuantity::parse(*val, pair.back());

		return std::nullopt;
	}
};


// --- Macros ---

#define _CHG_DYN_CONVERTIBLE_UNITS_ENTRY(From, To)               \
    { UnitId::of<From>(), &DynamicQuantity::convert<From, To> }, \

// Enables a bidirectional dynamic conversion between 2 units.
#define CHG_DYN_CONVERTIBLE_UNITS_2(A, B)                                             \
	template <>                                                                       \
    inline const DynamicQuantity::ConverterMap& DynamicQuantity::getConvertersTo<A>() \
    {                                                                                 \
        static const ConverterMap converters                                          \
        {                                                                             \
			_CHG_DYN_CONVERTIBLE_UNITS_ENTRY(B, A)                                    \
        };                                                                            \
        return converters;                                                            \
    }                                                                                 \
                                                                                      \
	template <>                                                                       \
    inline const DynamicQuantity::ConverterMap& DynamicQuantity::getConvertersTo<B>() \
    {                                                                                 \
        static const ConverterMap converters                                          \
        {                                                                             \
			_CHG_DYN_CONVERTIBLE_UNITS_ENTRY(A, B)                                    \
        };                                                                            \
        return converters;                                                            \
    }                                                                                 \

// Enables a bidirectional dynamic conversion between 3 units.
#define CHG_DYN_CONVERTIBLE_UNITS_3(A, B, C)                                          \
	template <>                                                                       \
    inline const DynamicQuantity::ConverterMap& DynamicQuantity::getConvertersTo<A>() \
    {                                                                                 \
        static const ConverterMap converters                                          \
        {                                                                             \
			_CHG_DYN_CONVERTIBLE_UNITS_ENTRY(B, A)                                    \
			_CHG_DYN_CONVERTIBLE_UNITS_ENTRY(C, A)                                    \
        };                                                                            \
        return converters;                                                            \
    }                                                                                 \
                                                                                      \
	template <>                                                                       \
    inline const DynamicQuantity::ConverterMap& DynamicQuantity::getConvertersTo<B>() \
    {                                                                                 \
        static const ConverterMap converters                                          \
        {                                                                             \
			_CHG_DYN_CONVERTIBLE_UNITS_ENTRY(A, B)                                    \
			_CHG_DYN_CONVERTIBLE_UNITS_ENTRY(C, B)                                    \
        };                                                                            \
        return converters;                                                            \
    }                                                                                 \
                                                                                      \
	template <>                                                                       \
    inline const DynamicQuantity::ConverterMap& DynamicQuantity::getConvertersTo<C>() \
    {                                                                                 \
        static const ConverterMap converters                                          \
        {                                                                             \
			_CHG_DYN_CONVERTIBLE_UNITS_ENTRY(A, C)                                    \
			_CHG_DYN_CONVERTIBLE_UNITS_ENTRY(B, C)                                    \
        };                                                                            \
        return converters;                                                            \
    }                                                                                 \

// Enables a bidirectional dynamic conversion between 4 units.
#define CHG_DYN_CONVERTIBLE_UNITS_4(A, B, C, D)                                       \
	template <>                                                                       \
    inline const DynamicQuantity::ConverterMap& DynamicQuantity::getConvertersTo<A>() \
    {                                                                                 \
        static const ConverterMap converters                                          \
        {                                                                             \
			_CHG_DYN_CONVERTIBLE_UNITS_ENTRY(B, A)                                    \
			_CHG_DYN_CONVERTIBLE_UNITS_ENTRY(C, A)                                    \
            _CHG_DYN_CONVERTIBLE_UNITS_ENTRY(D, A)                                    \
        };                                                                            \
        return converters;                                                            \
    }                                                                                 \
                                                                                      \
	template <>                                                                       \
    inline const DynamicQuantity::ConverterMap& DynamicQuantity::getConvertersTo<B>() \
    {                                                                                 \
        static const ConverterMap converters                                          \
        {                                                                             \
			_CHG_DYN_CONVERTIBLE_UNITS_ENTRY(A, B)                                    \
			_CHG_DYN_CONVERTIBLE_UNITS_ENTRY(C, B)                                    \
            _CHG_DYN_CONVERTIBLE_UNITS_ENTRY(D, B)                                    \
        };                                                                            \
        return converters;                                                            \
    }                                                                                 \
                                                                                      \
	template <>                                                                       \
    inline const DynamicQuantity::ConverterMap& DynamicQuantity::getConvertersTo<C>() \
    {                                                                                 \
        static const ConverterMap converters                                          \
        {                                                                             \
			_CHG_DYN_CONVERTIBLE_UNITS_ENTRY(A, C)                                    \
			_CHG_DYN_CONVERTIBLE_UNITS_ENTRY(B, C)                                    \
            _CHG_DYN_CONVERTIBLE_UNITS_ENTRY(D, C)                                    \
        };                                                                            \
        return converters;                                                            \
    }                                                                                 \
                                                                                      \
	template <>                                                                       \
    inline const DynamicQuantity::ConverterMap& DynamicQuantity::getConvertersTo<D>() \
    {                                                                                 \
        static const ConverterMap converters                                          \
        {                                                                             \
			_CHG_DYN_CONVERTIBLE_UNITS_ENTRY(A, D)                                    \
			_CHG_DYN_CONVERTIBLE_UNITS_ENTRY(B, D)                                    \
            _CHG_DYN_CONVERTIBLE_UNITS_ENTRY(C, D)                                    \
        };                                                                            \
        return converters;                                                            \
    }                                                                                 \
