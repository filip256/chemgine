#include "Spline.hpp"
#include "Precision.hpp"
#include "NumericUtils.hpp"

#include <limits>
#include <algorithm>

template <typename T>
Spline<T>::Spline(
	std::vector<std::pair<T, T>>&& points,
	const T maxCompressionLoss
) noexcept
{
	std::sort(points.begin(), points.end(), [](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });
	points.erase(std::unique(
			points.begin(),points.end(),
		[](const auto& lhs, const auto& rhs) { return Utils::floatEqual(lhs.first, rhs.first); }),
		points.end());

	this->points = std::move(points);

	compress(maxCompressionLoss);
}

template <typename T>
Spline<T>::Spline(
	std::initializer_list<std::pair<T, T>> initializer,
	const T maxCompressionLoss
) noexcept :
	Spline(std::vector<std::pair<T, T>>(initializer), maxCompressionLoss)
{}

template <typename T>
T Spline<T>::getSlope(const size_t i, const size_t j) const
{
	return (points[i].second - points[j].second) / (points[i].first - points[j].first);
}

template <typename T>
size_t Spline<T>::size() const
{
	return points.size();
}

template <typename T>
const std::vector<std::pair<T, T>>& Spline<T>::getContent() const
{
	return points;
}

template <typename T>
const std::pair<T, T>& Spline<T>::front() const
{
	return points.front();
}

template <typename T>
const std::pair<T, T>& Spline<T>::back() const
{
	return points.back();
}

template <typename T>
size_t Spline<T>::getUpperBound(const T x) const
{
	// considering the small number of points in most splines, binary search is probably less efficient
	for (size_t i = 0; i < points.size(); ++i)
		if (points[i].first > x)
			return i;

	return npos;
}

template <typename T>
T Spline<T>::getLinearValueAt(const T x) const
{
	if (points.size() == 1)
		return points.front().second;

	size_t hb = getUpperBound(x);

	if (hb == 0)
		hb = 1;

	if (hb == npos)
		hb = points.size() - 1;

	const T slope = getSlope(hb - 1, hb);
	return slope * (x - points[hb].first) + points[hb].second;
}

template <typename T>
T Spline<T>::getQuadraticValueAt(const T x) const
{
	if (points.size() < 3)
		return getLinearValueAt(x);

	// https://www.geeksforgeeks.org/quadratic-interpolation/

	return 0;
}

template <typename T>
void Spline<T>::compress(const T maxLinearError)
{
	if (maxLinearError < 0.0 || points.size() < 2)
		return;

	size_t i = points.size() - 2;
	while (i > 0)
	{
		const T slope = getSlope(i - 1, i + 1);
		const T after = (slope * (points[i].first - points[i - 1].first) + points[i - 1].second);
		if (Utils::floatEqual(points[i].second, after, maxLinearError))
		{
			points.erase(points.begin() + i);
			if (points.size() < 3)
				break;
		}
		--i;
	}

	if (points.size() == 2 && Utils::floatEqual(points.front().second, points.back().second, maxLinearError))
		points.pop_back();

	points.shrink_to_fit();
}

template <typename T>
bool Spline<T>::isEquivalent(const Spline<T>& other, const T epsilon) const
{
	for (size_t i = 0; i < this->points.size(); ++i)
		if (not Utils::floatEqual(this->points[i].second, other.getLinearValueAt(this->points[i].first), epsilon))
			return false;
	return true;
}

template class Spline<float_n>;
