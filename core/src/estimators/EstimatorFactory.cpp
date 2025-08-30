#include "estimators/EstimatorFactory.hpp"

EstimatorFactory::EstimatorFactory(EstimatorRepository& repository) noexcept :
    repository(repository)
{}
