#pragma once

#include "estimators/kinds/EstimatorBase.hpp"

#include <memory>
#include <unordered_map>

class EstimatorRepository
{
private:
    uint16_t                                                              maxEstimatorNesting = 0;
    std::unordered_map<EstimatorId, std::unique_ptr<const EstimatorBase>> estimators;

    EstimatorId getFreeId() const;

    const EstimatorBase& add(std::unique_ptr<const EstimatorBase>&& estimator);

public:
    EstimatorRepository()                           = default;
    EstimatorRepository(const EstimatorRepository&) = delete;
    EstimatorRepository(EstimatorRepository&&)      = default;
    ~EstimatorRepository() noexcept;

    /// <summary>
    /// Builds a new estimator of the given type.
    /// </summary>
    template <typename EstT, typename... Args>
    CountedRef<const EstT> add(Args&&... args);

    void dropUnusedEstimators();

    bool                 contains(const EstimatorId id) const;
    const EstimatorBase& at(const EstimatorId id) const;

    size_t totalDefinitionCount() const;

    using Iterator = std::unordered_map<EstimatorId, std::unique_ptr<const EstimatorBase>>::const_iterator;
    Iterator begin() const;
    Iterator end() const;

    void clear();
};

template <typename EstT, typename... Args>
CountedRef<const EstT> EstimatorRepository::add(Args&&... args)
{
    static_assert(
        std::is_base_of_v<EstimatorBase, EstT>, "EstimatorRepository: EstT must be an EstimatorBase derived type.");

    const auto id = getFreeId();
    return static_cast<const EstT&>(add(std::make_unique<EstT>(id, std::forward<Args>(args)...)));
}
