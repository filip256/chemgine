#pragma once

#include "EstimatorBase.hpp"

#include <memory>
#include <unordered_map>

class EstimatorRepository
{
private:
	std::unordered_map<EstimatorId, std::unique_ptr<const EstimatorBase>> table;

	EstimatorId getFreeId() const;

	const EstimatorBase& add(std::unique_ptr<const EstimatorBase>&& estimator);

public:
	EstimatorRepository() = default;
	EstimatorRepository(const EstimatorRepository&) = delete;

	/// <summary>
	/// Constructs and allocates a new estimator of the given type.
	/// Specializations can provide optimizations for certain estimator types.
	/// </summary>
	template <typename EstT, typename... Args>
	const EstT& add(Args&&... args);

	const EstimatorBase& at(const EstimatorId id) const;
};

template <typename EstT, typename... Args>
const EstT& EstimatorRepository::add(Args&&... args)
{
	static_assert(std::is_base_of_v<EstimatorBase, EstT>,
		"EstimatorRepository: EstT must be a EstimatorBase derived type.");
	static_assert(std::is_constructible_v<EstT, EstimatorId, Args... >,
		"EstimatorRepository: Unable to construct EstT from the given Args.");

	const auto id = getFreeId();
	return static_cast<const EstT&>(add(std::make_unique<EstT>(id, std::forward<Args>(args)...)));
}
