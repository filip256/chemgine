#pragma once

#include "structs/Point.hpp"

/// <summary>
/// Horizontal line represented by an origin point and a positive length on the Ox axis.
/// </summary>
template<typename T, typename LenT = utils::float_or_unsigned_t<T>>
class HLine
{
private:
    T endX() const;

public:
    LenT length;
    Point<T> origin;

    constexpr HLine(const Point<T>& origin, const LenT length) noexcept;
    constexpr HLine(const HLine&) = default;

    template<typename OtherT, typename OtherLenT = utils::float_or_unsigned_t<OtherT>,
        typename = std::enable_if_t<is_safe_conversion_v<OtherT, T>>>
    constexpr HLine(const HLine<OtherT, OtherLenT>& other) noexcept;
    template<typename OtherT, typename OtherLenT = utils::float_or_unsigned_t<T>>
    explicit constexpr HLine(const HLine<OtherT, OtherLenT>& other) noexcept;

    template<typename OtherT = T, typename OtherLenT = utils::float_or_unsigned_t<OtherT>>
    constexpr bool contains(const Point<OtherT>& point) const;
    template<typename OtherT = T, typename OtherLenT = utils::float_or_unsigned_t<OtherT>>
    constexpr bool intersects(const HLine<OtherT, OtherLenT>& other) const;

    constexpr Point<T> endPoint() const;

    constexpr void expandLeft(const LenT offset);
    constexpr void expandRight(const LenT offset);

    template<typename OtherT = T>
    constexpr Point<T> closestTo(const Point<OtherT>& point) const;
    template<typename OtherT = T, typename OtherLenT = utils::float_or_unsigned_t<OtherT>>
    constexpr std::pair<Point<T>, Point<OtherT>> closestTo(const HLine<OtherT, OtherLenT>& other) const;
};

template<typename T, typename LenT>
constexpr HLine<T, LenT>::HLine(const Point<T>& origin, const LenT length) noexcept :
    origin(origin),
    length(length)
{
    if constexpr (not std::is_unsigned_v<LenT>)
    {
        if (length < 0)
        {
            // Swap the origin and end-point such that length becomes positive.
            this->origin = Point<T>(origin.x - length + utils::SmallestPositive<T>, origin.y);
            this->length = -length;
        }
    }
}

template<typename T, typename LenT>
template<typename OtherT, typename OtherLenT, typename>
constexpr HLine<T, LenT>::HLine(const HLine<OtherT, OtherLenT>& other) noexcept :
    HLine(other.origin, other.length)
{}

template<typename T, typename LenT>
template<typename OtherT, typename OtherLenT>
constexpr HLine<T, LenT>::HLine(const HLine<OtherT, OtherLenT>& other) noexcept :
    HLine(other.origin, other.length)
{}

template<typename T, typename LenT>
template<typename OtherT, typename OtherLenT>
constexpr bool HLine<T, LenT>::contains(const Point<OtherT>& point) const
{
    return point.y == origin.y && point.x >= origin.x && point.x < origin.x + length;
}

template<typename T, typename LenT>
template<typename OtherT, typename OtherLenT>
constexpr bool HLine<T, LenT>::intersects(const HLine<OtherT, OtherLenT>& other) const
{
    return other.origin.y == this->origin.y && other.origin.x < this->origin.x + this->length && this->origin.x < other.origin.x + other.length;
}

template<typename T, typename LenT>
T HLine<T, LenT>::endX() const
{
    return origin.x + length - utils::SmallestPositive<T>;
}

template<typename T, typename LenT>
constexpr Point<T> HLine<T, LenT>::endPoint() const
{
    return Point<T>(endX(), origin.y);
}

template<typename T, typename LenT>
constexpr void HLine<T, LenT>::expandLeft(const LenT offset)
{
    origin.x -= offset;
}

template<typename T, typename LenT>
constexpr void HLine<T, LenT>::expandRight(const LenT offset)
{
    length += offset;
}

template<typename T, typename LenT>
template<typename OtherT>
constexpr Point<T> HLine<T, LenT>::closestTo(const Point<OtherT>& point) const
{
    return Point(
        std::min(static_cast<T>(endX()), std::max(origin.x, point.x)),
        origin.y);
}

template<typename T, typename LenT>
template<typename OtherT, typename OtherLenT>
constexpr std::pair<Point<T>, Point<OtherT>> HLine<T, LenT>::closestTo(const HLine<OtherT, OtherLenT>& other) const
{
    const auto thisEndX = this->endX();
    const auto otherEndX = other.endX();

    //    v       v     v
    // Tttt       Tttt Tttt
    //    Oooo Oooo     Oooo
    return
        thisEndX <= other.origin.x ? std::make_pair(
            Point<T>(thisEndX, this->origin.y), other.origin) :
        otherEndX <= this->origin.x ? std::make_pair(
            this->origin, Point<OtherT>(otherEndX, other.origin.y)) :
        this->origin.x >= other.origin.x ? std::make_pair(
            this->origin, Point<OtherT>(static_cast<OtherT>(this->origin.x), other.origin.y)) :
        std::make_pair(
            Point<T>(static_cast<T>(other.origin.x), this->origin.y), other.origin);
}

//
// Extras
//

template<typename T, typename LenT>
struct std::formatter<HLine<T, LenT>>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const HLine<T, LenT>& line, FormatContext& ctx) const
    {
        return format_to(ctx.out(), "{0} -> {1}", line.origin, line.endPoint());
    }
};

template<typename T, typename LenT>
class utils::NPos<HLine<T, LenT>>
{
public:
    using ObjT = HLine<T, LenT>;

    static constexpr ObjT value = ObjT(NPos<Point<T>>::value, NPos<LenT>::value);
    static constexpr bool isNPos(const ObjT& line)
    {
        return utils::isNPos(line.origin);
    }
};
