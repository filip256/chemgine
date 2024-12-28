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

	template<typename SrcT, typename DstT>
	DynamicQuantity convert() const;
	template<typename DstT>
	inline static const ConverterMap& getConvertersTo();

	template<typename UnitT>
	static const std::string& symbolOf();
	template<typename UnitT>
	static const std::string& nameOf();

	using Converters = const ConverterMap& (*)();
	using Symbol = Utils::unique_func_t<const std::string& (*)(), 0>;
	using Name = Utils::unique_func_t<const std::string& (*)(), 1>;
	using UnitInfo = std::tuple<Converters, Symbol, Name>;

	template<typename UnitT>
	static constexpr inline std::pair<UnitId, UnitInfo> makeUnitInfo();

	template<typename InfoT>
	static const auto& getUnitInfo(const UnitId unit);
	static const UnitInfo& getUnitInfo(const UnitId unit);

public:
	DynamicQuantity(const float_q value, const UnitId unit) noexcept;
	DynamicQuantity(const DynamicQuantity&) = default;

	template<typename UnitT>
	DynamicQuantity(const Quantity<UnitT> quantity) noexcept;

	float_q value() const;

	template<typename DstT>
	std::optional<Quantity<DstT>> to() const;
	std::optional<DynamicQuantity> to(const UnitId target) const;

	const std::string& unitSymbol() const;
	const std::string& unitName() const;

	static const std::string& getUnitSymbol(const UnitId unit);
	static const std::string& getUnitName(const UnitId unit);

	static std::optional<UnitId> parseUnitSymbol(const std::string& symbol);

	template <typename UnitT>
	static std::optional<Quantity<UnitT>> parse(const float_q value, const std::string& symbol);
	static std::optional<DynamicQuantity> parse(const float_q value, const std::string& symbol);

	static const UnitId AnyUnitId;

	friend std::ostream& operator<<(std::ostream& os, const DynamicQuantity& quantity);
};

template<typename UnitT>
DynamicQuantity::DynamicQuantity(const Quantity<UnitT> quantity) noexcept :
	val(quantity.value()),
	unit(UnitId::of<UnitT>())
{}

template<typename DstT>
std::optional<Quantity<DstT>> DynamicQuantity::to() const
{
	if (unit == UnitId::of<DstT>() || unit == AnyUnitId)
		return Quantity<DstT>::from(val);

	const auto& converters = getConvertersTo<DstT>();

	const auto it = converters.find(unit);
	return it != converters.end() ?
		std::optional(Quantity<DstT>::from((this->*(it->second))().val)) :
		std::nullopt;
}

template<typename SrcT, typename DstT>
DynamicQuantity DynamicQuantity::convert() const
{
	return Quantity<SrcT>::from(val).to<DstT>();
}

template<typename SrcT>
inline const DynamicQuantity::ConverterMap& DynamicQuantity::getConvertersTo()
{
	static const ConverterMap empty{};
	return empty;
}

template<typename UnitT>
const std::string& DynamicQuantity::symbolOf()
{
	return Quantity<UnitT>::getUnitSymbol();
}

template<typename UnitT>
const std::string& DynamicQuantity::nameOf()
{
	return Quantity<UnitT>::getUnitName();
}

template<typename UnitT>
constexpr inline std::pair<UnitId, DynamicQuantity::UnitInfo> DynamicQuantity::makeUnitInfo()
{
	return std::pair(UnitId::of<UnitT>(), std::tuple(getConvertersTo<UnitT>, symbolOf<UnitT>, nameOf<UnitT>));
}

template<typename InfoT>
const auto& DynamicQuantity::getUnitInfo(const UnitId unit)
{
	return std::get<InfoT>(getUnitInfo(unit))();
}

template <typename UnitT>
std::optional<Quantity<UnitT>> DynamicQuantity::parse(const float_q value, const std::string& symbol)
{
	const auto dynQuantity = DynamicQuantity::parse(value, symbol);
	return dynQuantity ?
		std::optional(dynQuantity->to<UnitT>()) :
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


template <typename UnitT>
class Def::Parser<Quantity<UnitT>>
{
public:
	static std::optional<Quantity<UnitT>> parse(const std::string& str)
	{
		const auto pair = Utils::split(Utils::strip(str), '_', true);
		if (pair.empty())
			return std::nullopt;

		const auto val = Def::parse<float_q>(Utils::strip(pair.front()));
		if (not val)
			return std::nullopt;

		if (pair.size() == 1)
			return Quantity<UnitT>::from(*val);

		if (pair.size() == 2)
			return DynamicQuantity::parse<UnitT>(*val, pair.back());

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
