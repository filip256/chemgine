#pragma once

#include <typeinfo>
#include <cassert>

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

// #define SKIP_CHECKED_CASTS

/// <summary>
/// Similar to static_cast but assures no data is lost during the conversion.
/// </summary>
template<typename DstT, typename SrcT>
DstT checked_cast(const SrcT& src)
{
#ifdef SKIP_CHECKED_CASTS
	return static_cast<DstT>(src);
#else
	const auto dst = static_cast<DstT>(src);
	assert((src == static_cast<SrcT>(dst)) && "Checked cast failed.");
	return dst;
#endif
}

/// <summary>
/// Casts an enum to it's underlying type.
/// </summary>
template<typename EnumT>
constexpr inline auto underlying_cast(const EnumT enumValue) noexcept
{
	static_assert(std::is_enum_v<EnumT>, "EnumT must be an enum type.");
	return static_cast<std::underlying_type_t<EnumT>>(enumValue);
}
