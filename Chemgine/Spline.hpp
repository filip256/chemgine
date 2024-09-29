#pragma once

#include "Parsers.hpp"

#include <vector>

template <class T>
class Spline
{
private:
	std::vector<std::pair<T, T>> points;

	T getSlope(const size_t i, const size_t j) const;

public:
	using StorageType = T;

	Spline() = default;
	Spline(
		std::vector<std::pair<T, T>>&& points,
		const T maxCompressionLoss = 0.0
	) noexcept;
	Spline(
		std::initializer_list<std::pair<T, T>> initializer,
		const T maxCompressionLoss = 0.0
	) noexcept;
	Spline(const Spline&) = default;
	Spline(Spline&&) = default;

	Spline& operator=(Spline&&) = default;

	const std::pair<T, T>& front() const;
	const std::pair<T, T>& back() const;
	size_t size() const;
	size_t getUpperBound(const T x) const;

	T getLinearValueAt(const T x) const;
	T getQuadraticValueAt(const T x) const;

	/// <summary>
	/// Removes as many entries as possible, making sure that the max error between
	/// the linear interpolations of the original and the result is less or equal to
	/// maxLinearError.
	/// </summary>
	void compress(const T maxLinearError);

	bool isEquivalent(const Spline<T>& other,
		const T epsilon = std::numeric_limits<T>::epsilon()
	) const;

	static constexpr size_t npos = static_cast<size_t>(-1);
};


template <typename T>
class Def::Parser<Spline<T>>
{
public:
	static std::optional<Spline<T>> parse(const std::string& str)
	{
		const auto& pointsStr = Utils::split(str, ',', true);

		std::vector<std::pair<float, float>> points;
		points.reserve(pointsStr.size());

		for (size_t i = 0; i < pointsStr.size(); ++i)
		{
			const auto p = Def::parse<std::pair<double, double>>(pointsStr[i]);
			if (not p)
				return std::nullopt;
			points.emplace_back(Utils::reversePair(*p));
		}

		return points.empty() ? std::nullopt : std::optional<Spline<float>>(Spline(std::move(points)));
	}
};
