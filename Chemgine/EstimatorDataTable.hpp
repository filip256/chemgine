#pragma once

#include "BaseEstimator.hpp"
#include "FunctionalEstimator.hpp"
#include "SplineEstimator.hpp"
#include "LinearEstimator.hpp"
#include "ConstantEstimator.hpp"
#include "Maths.hpp"

#include <unordered_map>

enum class BuiltinEstimator : EstimatorIdType
{
	TEMP_TO_REL_RSPEED = 101,
	MCONC_TO_REL_RSPEED = 102,
	TEMP_TO_DENSITY = 103,
	TORR_TO_REL_BP = 104,
	TDIF_TORR_TO_REL_LH = 105,
	TEMP_TO_REL_SOL = 106,
	TEMP_TO_REL_INV_SOL = 107,
};

inline constexpr EstimatorIdType toId(const BuiltinEstimator tag);


class EstimatorDataTable
{
private:
	std::unordered_map<EstimatorIdType, const BaseEstimator*> table;

	EstimatorIdType getFreeId() const;

	void addPredefined();
	const BaseEstimator& add(const BaseEstimator* estimator);

public:
	EstimatorDataTable() = default;
	EstimatorDataTable(const EstimatorDataTable&) = delete;
	~EstimatorDataTable() noexcept;

	bool loadFromFile(const std::string& path);
	
	/// <summary>
	/// Allocates a new estimator and appends it to the table.
	/// Specializations can provide optimizations for certain estimators.
	/// </summary>
	template <typename EstT, typename... Args, typename = std::enable_if_t<
		std::is_base_of_v<BaseEstimator, EstT> &&
		std::is_constructible_v<EstT, EstimatorIdType, Args...>>>
	const BaseEstimator& add(Args&&... args);
	//  and they still ask why I use C++...

	const BaseEstimator& at(const EstimatorIdType id) const;
};


template <typename EstT, typename... Args, typename>
const BaseEstimator& EstimatorDataTable::add(Args&&... args)
{
	const auto id = getFreeId();
	const auto* estimator = new EstT(id, std::forward<Args>(args)...);
	return add(estimator);
}

// --- Estimator optimizers:
template <>
inline const BaseEstimator& EstimatorDataTable::add<SplineEstimator, Spline<float>>(Spline<float>&& spline)
{
	if (spline.size() == 1)
		return add<ConstantEstimator>(spline.front().second);

	if (spline.size() == 2)
	{
		const auto temp = Maths::getSlopeAndIntercept(
			spline.front().first, spline.front().second,
			spline.back().first, spline.back().second);
		return add<LinearEstimator>(temp.first, temp.second);
	}

	const auto id = getFreeId();
	const auto* estimator = new SplineEstimator(id, std::move(spline));
	return add(estimator);
}

template <>
inline const BaseEstimator& EstimatorDataTable::add<LinearEstimator, double, double>(double&& scale, double&& offset)
{
	if (scale - 1.0 < std::numeric_limits<double>::epsilon())
		return add<ConstantEstimator>(offset);

	const auto id = getFreeId();
	const auto* estimator = new LinearEstimator(id, scale, offset);
	return add(estimator);
}