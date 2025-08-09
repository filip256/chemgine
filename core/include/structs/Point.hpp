#pragma once

#include "utils/Math.hpp"
#include "utils/STL.hpp"
#include "utils/Hash.hpp"
#include "utils/Casts.hpp"

#include <utility>
#include <format>

template<typename T>
class Point
{
	static_assert(std::is_arithmetic_v<T>, "Point: T must be an arithmetic type.");

public:
	T x, y;

	constexpr Point(const T x,const T y) noexcept;
	template <typename OtherT>
	explicit constexpr Point(const OtherT x, const OtherT y) noexcept;

	constexpr Point(const std::pair<T, T>& pair) noexcept;
	template <typename OtherT, typename = std::enable_if_t<is_safe_conversion_v<OtherT, T>>>
	constexpr Point(const std::pair<OtherT, OtherT>& pair) noexcept;
	template <typename OtherT>
	explicit constexpr Point(const std::pair<OtherT, OtherT>& pair) noexcept;

	constexpr Point(const Point&) = default;
	template<typename OtherT, typename = std::enable_if_t<is_safe_conversion_v<OtherT, T>>>
	constexpr Point(const Point<OtherT>& other) noexcept;
	template<typename OtherT>
	explicit constexpr Point(const Point<OtherT>& other) noexcept;

	template <typename OtherT = T>
	bool operator==(const Point<OtherT>& other) const;
	template <typename OtherT = T>
	bool operator!=(const Point<OtherT>& other) const;

	Point& operator=(const Point& other) = default;

	template<typename OtherT = T, typename OutT = decltype(std::declval<T>() + std::declval<OtherT>())>
	Point<OutT> operator+(const Point<OtherT>& other) const;
	template<typename OtherT = T>
	Point& operator+=(const Point<OtherT>& other);
	template<typename OtherT = T, typename OutT = decltype(std::declval<T>() - std::declval<OtherT>())>
	Point<OutT> operator-(const Point<OtherT>& other) const;
	template<typename OtherT = T>
	Point& operator-=(const Point<OtherT>& other);

	template<typename ScalarT = T, typename OutT = decltype(std::declval<T>() * std::declval<ScalarT>())>
	Point<OutT> operator*(const ScalarT scalar) const;
	template<typename ScalarT = T>
	Point& operator*=(const ScalarT scalar);
	template<typename ScalarT = T, typename OutT = decltype(std::declval<T>() / std::declval<ScalarT>())>
	Point<OutT> operator/(const ScalarT scalar) const;
	template<typename ScalarT = T>
	Point& operator/=(const ScalarT scalar);

	Point<utils::float_or_unsigned_t<T>> abs() const;
	Point transpose() const;

	template <typename OtherT = T>
	auto dot(const Point<OtherT>& other) const;
	template <typename OtherT = T>
	auto cross(const Point<OtherT>& other) const;
	template <typename OtherT = T>
	auto squaredDistance(const Point<OtherT>& other) const;
	template <typename OtherT = T>
	auto chebyshevDistance(const Point<OtherT>& other) const;
};

template<typename T>
constexpr Point<T>::Point(const T x, const T y) noexcept :
	x(x),
	y(y)
{}

template<typename T>
template<typename OtherT>
constexpr Point<T>::Point(const OtherT x, const OtherT y) noexcept :
	Point(static_cast<T>(x), static_cast<T>(y))
{}

template<typename T>
constexpr Point<T>::Point(const std::pair<T, T>& pair) noexcept :
	Point(pair.first, pair.second)
{}

template<typename T>
template<typename OtherT, typename>
constexpr Point<T>::Point(const std::pair<OtherT, OtherT>& pair) noexcept :
	Point(pair.first, pair.second)
{}

template<typename T>
template<typename OtherT>
constexpr Point<T>::Point(const std::pair<OtherT, OtherT>& pair) noexcept :
	Point(pair.first, pair.second)
{}

template<typename T>
template<typename OtherT, typename>
constexpr Point<T>::Point(const Point<OtherT>& other) noexcept :
	Point(other.x, other.y)
{}

template<typename T>
template<typename OtherT>
constexpr Point<T>::Point(const Point<OtherT>& other) noexcept :
	Point(other.x, other.y)
{}

template<typename T>
template<typename OtherT>
bool Point<T>::operator==(const Point<OtherT>& other) const
{
	return this->x == other.x && this->y == other.y;
}

template<typename T>
template<typename OtherT>
bool Point<T>::operator!=(const Point<OtherT>& other) const
{
	return this->x != other.x || this->y != other.y;
}

template<typename T>
template<typename OtherT, typename OutT>
Point<OutT> Point<T>::operator+(const Point<OtherT>& other) const
{
	return Point<OutT>(this->x + other.x, this->y + other.y);
}

template<typename T>
template<typename OtherT>
Point<T>& Point<T>::operator+=(const Point<OtherT>& other)
{
	this->x += other.x;
	this->y += other.y;
	return *this;
}

template<typename T>
template<typename OtherT, typename OutT>
Point<OutT> Point<T>::operator-(const Point<OtherT>& other) const
{
	return Point<OutT>(this->x - other.x, this->y - other.y);
}

template<typename T>
template<typename OtherT>
Point<T>& Point<T>::operator-=(const Point<OtherT>& other)
{
	this->x -= other.x;
	this->y -= other.y;
	return *this;
}

template<typename T>
template<typename ScalarT, typename OutT>
Point<OutT> Point<T>::operator*(const ScalarT scalar) const
{
	return Point<OutT>(x * scalar, y * scalar);
}

template<typename T>
template<typename ScalarT>
Point<T>& Point<T>::operator*=(const ScalarT scalar)
{
	x = static_cast<T>(x * scalar);
	y = static_cast<T>(y * scalar);
	return *this;
}

template<typename T>
template<typename ScalarT, typename OutT>
Point<OutT> Point<T>::operator/(const ScalarT scalar) const
{
	return Point<OutT>(x / scalar, y / scalar);
}

template<typename T>
template<typename ScalarT>
Point<T>& Point<T>::operator/=(const ScalarT scalar)
{
	x = static_cast<T>(x / scalar);
	y = static_cast<T>(y / scalar);
	return *this;
}

template<typename T>
Point<utils::float_or_unsigned_t<T>> Point<T>::abs() const
{
	return Point<utils::float_or_unsigned_t<T>>(utils::abs(x), utils::abs(y));
}

template<typename T>
Point<T> Point<T>::transpose() const
{
	return Point(y, x);
}

template<typename T>
template<typename OtherT>
auto Point<T>::dot(const Point<OtherT>& other) const
{
	return utils::dotProduct(this->x, this->y, other.x, other.y);
}

template<typename T>
template<typename OtherT>
auto Point<T>::cross(const Point<OtherT>& other) const
{
	return utils::crossProduct(this->x, this->y, other.x, other.y);
}

template<typename T>
template<typename OtherT>
auto Point<T>::squaredDistance(const Point<OtherT>& other) const
{
	return utils::squaredDistance(this->x, this->y, other.x, other.y);
}

template<typename T>
template<typename OtherT>
auto Point<T>::chebyshevDistance(const Point<OtherT>& other) const
{
	return utils::chebyshevDistance(this->x, this->y, other.x, other.y);
}

//
// Extras
//

template<typename T>
struct std::formatter<Point<T>>
{
	constexpr auto parse(format_parse_context& ctx)
	{
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const Point<T>& point, FormatContext& ctx) const
	{
		return format_to(ctx.out(), "({0}, {1})", point.x, point.y);
	}
};

template <typename T>
class utils::NPos<Point<T>>
{
public:
	using ObjT = Point<T>;

	static constexpr ObjT value = ObjT(NPos<T>::value, NPos<T>::value);
	static constexpr bool isNPos(const ObjT& point)
	{
		return utils::isNPos(point.x);
	}
};

template<typename T>
struct std::hash<Point<T>>
{
	size_t operator() (const Point<T>& point) const
	{
		if constexpr (sizeof(T) * 2 <= sizeof(size_t))
			return (static_cast<size_t>(point.x) << sizeof(T)) | point.y;
		else
			return utils::hashCombine(point.x, point.y);
	}
};

template<typename DstT, typename SrcT>
Point<DstT> checked_cast(const Point<SrcT>& src)
{
	return Point(checked_cast<DstT>(src.x), checked_cast<DstT>(src.y));
}

template<typename DstT, typename SrcT>
Point<DstT> round_cast(const Point<SrcT>& src)
{
	return Point(round_cast<DstT>(src.x), round_cast<DstT>(src.y));
}
