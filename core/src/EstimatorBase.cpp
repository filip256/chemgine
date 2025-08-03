#include "EstimatorBase.hpp"
#include "Log.hpp"
#include "FormatUtils.hpp"

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

bool EstimatorBase::isEquivalent(const EstimatorBase& other, const float_s epsilon) const
{
	return typeid(*this) == typeid(other);
}

uint16_t EstimatorBase::getNestingDepth() const
{
	return 0;
}

void EstimatorBase::print(std::ostream& out) const
{
	std::unordered_set<EstimatorId> history;
	dumpDefinition(out, true, history, false, 0);
}
