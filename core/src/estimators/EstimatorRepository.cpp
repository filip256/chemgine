#include "estimators/EstimatorRepository.hpp"

#include "data/def/Keywords.hpp"
#include "data/def/Parsers.hpp"
#include "io/Log.hpp"

#include <cmath>

EstimatorRepository::~EstimatorRepository() noexcept { clear(); }

const EstimatorBase& EstimatorRepository::add(std::unique_ptr<const EstimatorBase>&& estimator)
{
    const auto it = std::find_if(estimators.cbegin(), estimators.cend(), [&estimator](const auto& e) {
        return e.second->isEquivalent(*estimator);
    });

    if (it != estimators.end())
        return *it->second;

    maxEstimatorNesting = std::max(maxEstimatorNesting, estimator->getNestingDepth());

    const auto inserted = estimators.emplace(estimator->getId(), std::move(estimator));
    return *inserted.first->second;
}

void EstimatorRepository::dropUnusedEstimators()
{
    std::erase_if(estimators, [](const auto& p) { return p.second->getRefCount() == 0; });
}

bool EstimatorRepository::contains(const EstimatorId id) const { return estimators.contains(id); }

const EstimatorBase& EstimatorRepository::at(const EstimatorId id) const { return *estimators.at(id); }

size_t EstimatorRepository::totalDefinitionCount() const { return estimators.size(); }

EstimatorRepository::Iterator EstimatorRepository::begin() const { return estimators.begin(); }

EstimatorRepository::Iterator EstimatorRepository::end() const { return estimators.end(); }

void EstimatorRepository::clear()
{
    if (maxEstimatorNesting == 0)
        estimators.clear();

    // Ensure referenced estimators are deleted after those which reference them
    for (; maxEstimatorNesting-- > 0;)
        dropUnusedEstimators();
    dropUnusedEstimators();
}

EstimatorId EstimatorRepository::getFreeId() const
{
    static EstimatorId id = 0;
    while (estimators.contains(id)) {
        if (id == std::numeric_limits<EstimatorId>::max())
            Log(this).fatal("Estimator id limit reached: {}.", id);
        ++id;
    }
    return id;
}
