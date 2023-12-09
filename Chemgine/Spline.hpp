#pragma once

#include <vector>

template <class T>
class Spline
{
protected:
	std::vector<std::pair<T, T>> points;

public:
	Spline(std::vector<std::pair<T, T>>&& points) noexcept;
	Spline(std::initializer_list<std::pair<T, T>> initializer) noexcept;
	Spline(const Spline&) = default;
	Spline(Spline&&) = default;

	size_t size() const;
	size_t getHigherBound(const T x) const;

	T getLinearValueAt(const T x) const;
	T getQuadraticValueAt(const T x) const;

	static constexpr size_t npos = static_cast<size_t>(-1);
};