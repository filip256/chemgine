#pragma once

#include "utils/Build.hpp"
#include "utils/Exception.hpp"
#include "utils/Meta.hpp"
#include "utils/Type.hpp"

#include <cassert>
#include <cmath>
#include <type_traits>
#include <typeinfo>

template <typename SrcT, typename DstT>
struct is_safe_conversion
{
    using S = utils::underlying_or_self_t<SrcT>;
    using D = utils::underlying_or_self_t<DstT>;

    static_assert(
        std::is_arithmetic_v<S> && std::is_arithmetic_v<D>, "is_safe_conversion: SrcT and D must be arithmetic types.");

private:
    static constexpr bool SameFormat        = std::is_integral_v<S> == std::is_integral_v<D>;
    static constexpr bool ToFloat           = std::is_integral_v<S> && std::is_floating_point_v<D>;
    static constexpr bool SameSign          = std::is_signed_v<S> == std::is_signed_v<D>;
    static constexpr bool ToSigned          = std::is_unsigned_v<S> && std::is_signed_v<D>;
    static constexpr bool HigherWidth       = sizeof(S) < sizeof(D);
    static constexpr bool SameOrHigherWidth = sizeof(S) <= sizeof(D);

public:
    static constexpr bool value =
        (SameFormat && ((SameSign && SameOrHigherWidth) || (ToSigned && HigherWidth))) || (ToFloat && HigherWidth);
};

template <typename SrcT, typename DstT>
constexpr bool is_safe_conversion_v = is_safe_conversion<SrcT, DstT>::value;

// Tests:
//  - same:
static_assert(is_safe_conversion_v<uint8_t, uint8_t>);
static_assert(is_safe_conversion_v<int8_t, int8_t>);
static_assert(is_safe_conversion_v<float, float>);
// - widening:
static_assert(is_safe_conversion_v<uint8_t, uint16_t>);
static_assert(is_safe_conversion_v<int8_t, int16_t>);
static_assert(is_safe_conversion_v<float, double>);
static_assert(is_safe_conversion_v<uint8_t, int16_t>);
// - safe format change:
static_assert(is_safe_conversion_v<uint16_t, float>);
static_assert(is_safe_conversion_v<int16_t, float>);
static_assert(is_safe_conversion_v<uint32_t, double>);
static_assert(is_safe_conversion_v<int32_t, double>);
// - narrowing:
static_assert(not is_safe_conversion_v<uint16_t, uint8_t>);
static_assert(not is_safe_conversion_v<int16_t, int8_t>);
static_assert(not is_safe_conversion_v<double, float>);
// - sign-loss:
static_assert(not is_safe_conversion_v<int8_t, uint8_t>);
static_assert(not is_safe_conversion_v<int8_t, uint16_t>);
// - range-loss:
static_assert(not is_safe_conversion_v<uint8_t, int8_t>);
// - precision-loss:
static_assert(not is_safe_conversion_v<float, uint64_t>);
static_assert(not is_safe_conversion_v<float, int64_t>);
static_assert(not is_safe_conversion_v<uint32_t, float>);
static_assert(not is_safe_conversion_v<int32_t, float>);

/// <summary>
/// Checks if the source object has the same typeid as DstT.
/// </summary>
template <typename DstT, typename SrcT>
bool final_is(SrcT& src)
{
    return typeid(src) == typeid(DstT);
}

/// <summary>
/// Performs a typeid-checked cast. Cheaper than dynamic_cast but can only downcast to final types.
/// </summary>
template <typename DstT, typename SrcT>
DstT* final_cast(SrcT& src)
{
    return final_is<DstT>(src) ? static_cast<DstT*>(&src) : nullptr;
}

/// <summary>
/// Performs a typeid-checked cast. Cheaper than dynamic_cast but can only downcast to final types.
/// </summary>
template <typename DstT, typename SrcT>
const DstT* final_cast(const SrcT& src)
{
    return final_is<DstT>(src) ? static_cast<const DstT*>(&src) : nullptr;
}

/// <summary>
/// Similar to static_cast but assures no data is lost during the conversion.
/// </summary>
template <typename DstT, typename SrcT>
constexpr DstT checked_cast(const SrcT& src)
{
    using S = utils::underlying_or_self_t<SrcT>;
    using D = utils::underlying_or_self_t<DstT>;

    const auto dst = static_cast<DstT>(src);

    if constexpr (chg::ENABLE_CHECKED_CASTS && not is_safe_conversion_v<S, D>) {
        if constexpr (std::is_floating_point_v<S> && std::is_floating_point_v<D>) {
            // Floating-point narrowing almost always results in precision loss, the check only verifies the value is in
            // the DstT range.
            if (src < std::numeric_limits<D>::lowest() || src > std::numeric_limits<D>::max())
                chg::fatal(
                    "Checked cast failed: {}({}) -> {}({}).",
                    utils::getTypeName<SrcT>(),
                    src,
                    utils::getTypeName<DstT>(),
                    dst);
        }
        else if constexpr (std::is_floating_point_v<D>) {
            // Integer to float may be unsafe for values larger than 2^(mantissa + 1). When compering int to float C++
            // promotes the integer to floating-point. The check is made using a roundtrip cast back to SrcT.
            if (src != static_cast<SrcT>(dst))
                chg::fatal(
                    "Checked cast failed: {}({}) -> {}({}).",
                    utils::getTypeName<SrcT>(),
                    src,
                    utils::getTypeName<DstT>(),
                    dst);
        }
        else {
            // For other cases a single equality check is sufficient (from C++20 onwards).
            if (src != dst)
                chg::fatal(
                    "Checked cast failed: {}({}) -> {}({}).",
                    utils::getTypeName<SrcT>(),
                    src,
                    utils::getTypeName<DstT>(),
                    dst);
        }
    }

    return dst;
}

/// <summary>
/// Casts an enum to it's underlying type.
/// </summary>
template <typename EnumT>
inline constexpr auto underlying_cast(const EnumT enumValue) noexcept
{
    static_assert(std::is_enum_v<EnumT>, "underlying_cast(): EnumT must be an enum type.");
    return static_cast<std::underlying_type_t<EnumT>>(enumValue);
}

/// <summary>
/// Performs a rounded cast from a floating-point value to an integral one.
/// </summary>
template <typename DstT, typename SrcT>
DstT round_cast(const SrcT src)
{
    static_assert(std::is_floating_point_v<SrcT>, "round_cast(): SrcT must be a floating-point type.");
    static_assert(std::is_integral_v<DstT>, "round_cast(): DstT must be an integral type.");

    if constexpr (sizeof(DstT) > sizeof(long))
        return static_cast<DstT>(std::llround(src));
    else
        return static_cast<DstT>(std::lround(src));
}
