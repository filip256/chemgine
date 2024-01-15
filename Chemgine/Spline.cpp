#include "Spline.hpp"

#include <limits>
#include <algorithm>

template <class T>
Spline<T>::Spline(std::vector<std::pair<T, T>>&& points) noexcept
{
	std::sort(points.begin(), points.end(), [](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });
	points.erase(std::unique(
			points.begin(),points.end(),
			[](const auto& lhs, const auto& rhs) { return std::abs(lhs.first - rhs.first) < std::numeric_limits<T>::epsilon(); }),
		points.end());

	this->points = std::move(points);
}

template <class T>
Spline<T>::Spline(std::initializer_list<std::pair<T, T>> initializer) noexcept :
	Spline(std::vector<std::pair<T, T>>(initializer))
{}

template <class T>
size_t Spline<T>::size() const
{
	return points.size();
}

template <class T>
const std::pair<T, T>& Spline<T>::front() const
{
	return points.front();
}

template <class T>
const std::pair<T, T>& Spline<T>::back() const
{
	return points.back();
}

template <class T>
size_t Spline<T>::getHigherBound(const T x) const
{
	// considering the small number of points in most splines, binary search is probably less efficient
	for (size_t i = 0; i < points.size(); ++i)
		if (points[i].first > x)
			return i;

	return npos;
}

template <class T>
T Spline<T>::getLinearValueAt(const T x) const
{
	if (points.size() == 1)
		return points.front().second;

	size_t hb = this->getHigherBound(x);

	if (hb == 0)
		hb = 1;

	if (hb == this->npos)
		hb = this->points.size() - 1;

	const T slope = (this->points[hb - 1].second - this->points[hb].second) / (this->points[hb - 1].first - this->points[hb].first);
	return slope * (x - this->points[hb].first) + this->points[hb].second;
}

template <class T>
T Spline<T>::getQuadraticValueAt(const T x) const
{
	if (points.size() < 3)
		return getLinearValueAt(x);

	// https://www.geeksforgeeks.org/quadratic-interpolation/


	return 0;
}



template class Spline<float>;