#pragma once

#include "BaseEstimator.hpp"
#include "FunctionalEstimator.hpp"
#include "SplineEstimator.hpp"
#include "LinearEstimator.hpp"
#include "ConstantEstimator.hpp"
#include "DefinitionObject.hpp"
#include "Maths.hpp"

#include <unordered_map>

enum class BuiltinEstimator : EstimatorId
{
	TEMP_TO_REL_RSPEED = 101,
	MCONC_TO_REL_RSPEED = 102,
	TEMP_TO_DENSITY = 103,
	TORR_TO_REL_BP = 104,
	TDIF_TORR_TO_REL_LH = 105,
	TEMP_TO_REL_SOL = 106,
	TEMP_TO_REL_INV_SOL = 107,
};

inline constexpr EstimatorId toId(const BuiltinEstimator tag);


class EstimatorRepository
{
private:
	std::unordered_map<EstimatorId, const BaseEstimator*> table;

	EstimatorId getFreeId() const;

	const BaseEstimator& add(const BaseEstimator* estimator);

public:
	EstimatorRepository() noexcept;
	EstimatorRepository(const EstimatorRepository&) = delete;
	~EstimatorRepository() noexcept;

	/// <summary>
	/// Constructs and allocates a new estimator of the given type.
	/// Specializations can provide optimizations for certain estimator types.
	/// </summary>
	template <typename EstT, typename... Args, typename = std::enable_if_t<
		std::is_base_of_v<BaseEstimator, EstT> &&
		std::is_constructible_v<EstT, EstimatorId, Args...>>>
	const BaseEstimator& add(Args&&... args);

	const BaseEstimator& at(const EstimatorId id) const;
};


template <typename EstT, typename... Args, typename>
const BaseEstimator& EstimatorRepository::add(Args&&... args)
{
	const auto id = getFreeId();
	const auto* estimator = new EstT(id, std::forward<Args>(args)...);
	return add(estimator);
}

// --- Estimator optimizers:
template <>
inline const BaseEstimator& EstimatorRepository::add<SplineEstimator, Spline<float>>(Spline<float>&& spline)
{
	float compFactor = 0.0001f;
	while (spline.size() > 100 && compFactor <= 0.1f)
	{
		spline.compress(compFactor);
		compFactor *= 10;
	}

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
inline const BaseEstimator& EstimatorRepository::add<LinearEstimator, double, double>(double&& scale, double&& offset)
{
	if (scale - 1.0 < std::numeric_limits<double>::epsilon())
		return add<ConstantEstimator>(offset);

	const auto id = getFreeId();
	const auto* estimator = new LinearEstimator(id, scale, offset);
	return add(estimator);
}
