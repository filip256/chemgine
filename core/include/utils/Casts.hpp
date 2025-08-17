#pragma once

#include <typeinfo>
#include <cassert>
#include <cmath>

template<typename SrcT, typename DstT>
struct is_safe_conversion
{
	// Allows safe conversions between:
	// intX_t   -> intY_t    iff X <= Y
	// uintX_t  -> uintY_t   iff X <= Y
	// uintX_t  -> intY_t    iff X < Y
	// floatX_t -> floatY_t  iff X <= Y
private:
	static constexpr bool bothIntegral= std::is_integral_v<SrcT> && std::is_integral_v<DstT>;
	static constexpr bool bothFloating = std::is_integral_v<SrcT> && std::is_integral_v<DstT>;
	static constexpr bool sameSignedness = std::is_signed_v<SrcT> == std::is_signed_v<DstT>;

public:
	static constexpr bool value = (bothIntegral || bothFloating) &&
		((sameSignedness && sizeof(SrcT) <= sizeof(DstT)) || (std::is_unsigned_v<SrcT> && sizeof(SrcT) < sizeof(DstT)));
};

template<typename SrcT, typename DstT>
constexpr bool is_safe_conversion_v = is_safe_conversion<SrcT, DstT>::value;

/// <summary>
/// Checks if the source object has the same typeid as DstT.
/// </summary>
template<typename DstT, typename SrcT>
bool final_is(SrcT& src)
{
	return typeid(src) == typeid(DstT);
}

/// <summary>
/// Performs a typeid-checked cast. Cheaper than dynamic_cast but can only downcast to final types.
/// </summary>
template<typename DstT, typename SrcT>
DstT* final_cast(SrcT& src)
{
	return final_is<DstT>(src) ?
		static_cast<DstT*>(&src) :
		nullptr;
}

/// <summary>
/// Performs a typeid-checked cast. Cheaper than dynamic_cast but can only downcast to final types.
/// </summary>
template<typename DstT, typename SrcT>
const DstT* final_cast(const SrcT& src)
{
	return final_is<DstT>(src) ?
		static_cast<const DstT*>(&src) :
		nullptr;
}

/// <summary>
/// Similar to static_cast but assures no data is lost during the conversion.
/// </summary>
template<typename DstT, typename SrcT>
DstT checked_cast(const SrcT& src)
{
#ifdef CHG_DISABLE_CHECKED_CASTS
	return static_cast<DstT>(src);
#else
	if constexpr (is_safe_conversion_v<DstT, SrcT>)
		return static_cast<DstT>(src);
	else
	{
		const auto dst = static_cast<DstT>(src);
		assert((src == static_cast<SrcT>(dst)) && "Checked cast failed.");
		return dst;
	}
#endif
}

/// <summary>
/// Casts an enum to it's underlying type.
/// </summary>
template<typename EnumT>
constexpr inline auto underlying_cast(const EnumT enumValue) noexcept
{
	static_assert(std::is_enum_v<EnumT>, "underlying_cast(): EnumT must be an enum type.");
	return static_cast<std::underlying_type_t<EnumT>>(enumValue);
}

/// <summary>
/// Performs a rounded cast from a floating-point value to an integral one.
/// </summary>
template<typename DstT, typename SrcT>
DstT round_cast(const SrcT src)
{
	static_assert(std::is_floating_point_v<SrcT>, "round_cast(): SrcT must be a floating-point type.");
	static_assert(std::is_integral_v<DstT>, "round_cast(): DstT must be an integral type.");

	if constexpr (sizeof(DstT) > sizeof(long))
		return static_cast<DstT>(std::llround(src));
	else
		return static_cast<DstT>(std::lround(src));
}
