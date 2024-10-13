#include "EstimatorBase.hpp"
#include "Log.hpp"
#include "Linguistics.hpp"

#include <typeinfo>

EstimatorBase::EstimatorBase(const EstimatorId id) noexcept :
	id(id)
{}

EstimatorId EstimatorBase::getId() const
{
	return id;
}

std::string EstimatorBase::getDefIdentifier() const
{
	return 'd' + std::to_string(id);
}

bool EstimatorBase::isEquivalent(const EstimatorBase& other, const float_n epsilon) const
{
	return typeid(*this) == typeid(other);
}
