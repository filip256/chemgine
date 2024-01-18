#pragma once

#include "BaseApproximator.hpp"
#include "FunctionalApproximator.hpp"
#include "SplineApproximator.hpp"
#include "LinearApproximator.hpp"
#include "ConstantApproximator.hpp"
#include "Maths.hpp"

#include <unordered_map>

enum class Approximators : ApproximatorIdType
{
	TEMP_TO_REL_RSPEED = 101,
	MCONC_TO_REL_RSPEED = 102,
	TEMP_TO_DENSITY = 103,
	TORR_TO_REL_BP = 104,
	TDIF_TORR_TO_REL_LH = 105
};

class ApproximatorDataTable
{
private:
	std::unordered_map<ApproximatorIdType, const BaseApproximator*> table;

	ApproximatorIdType getFreeId() const;

	void addPredefined();
	const BaseApproximator& add(const BaseApproximator* approximator);

public:
	ApproximatorDataTable() = default;
	ApproximatorDataTable(const ApproximatorDataTable&) = delete;
	~ApproximatorDataTable() noexcept;

	bool loadFromFile(const std::string& path);
	
	/// <summary>
	/// Allocates a new approximator and appends it to the table.
	/// Specializations can provide optimizations for certain approximators.
	/// </summary>
	template <typename AppxT, typename... Args, typename = std::enable_if_t<
		std::is_base_of_v<BaseApproximator, AppxT> &&
		std::is_constructible_v<AppxT, ApproximatorIdType, Args...>>>
	const BaseApproximator& add(Args&&... args);
	//  and they still ask why I use C++...

	const BaseApproximator& at(const ApproximatorIdType id) const;
};


template <typename AppxT, typename... Args, typename>
const BaseApproximator& ApproximatorDataTable::add(Args&&... args)
{
	const auto id = getFreeId();
	const auto* appx = new AppxT(id, std::forward<Args>(args)...);
	return add(appx);
}

// --- Approximator optimizers:
template <>
inline const BaseApproximator& ApproximatorDataTable::add<SplineApproximator, Spline<float>>(Spline<float>&& spline)
{
	if (spline.size() == 1)
		return add<ConstantApproximator>(spline.front().second);

	if (spline.size() == 2)
	{
		const auto temp = Maths::getSlopeAndIntercept(
			spline.front().first, spline.front().second,
			spline.back().first, spline.back().second);
		return add<LinearApproximator>(temp.first, temp.second);
	}

	const auto id = getFreeId();
	const auto* appx = new SplineApproximator(id, std::move(spline));
	return add(appx);
}

template <>
inline const BaseApproximator& ApproximatorDataTable::add<LinearApproximator, double, double>(double&& scale, double&& offset)
{
	if (scale - 1.0 < std::numeric_limits<double>::epsilon())
		return add<ConstantApproximator>(offset);

	const auto id = getFreeId();
	const auto* appx = new LinearApproximator(id, scale, offset);
	return add(appx);
}