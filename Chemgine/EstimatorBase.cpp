#include "EstimatorBase.hpp"
#include "Log.hpp"

#include <typeinfo>

size_t EstimatorBase::instanceCount = 0;

EstimatorBase::EstimatorBase(const EstimatorId id) noexcept :
	id(id)
{}

EstimatorId EstimatorBase::getId() const
{
	return id;
}

bool EstimatorBase::isEquivalent(const EstimatorBase& other, const double epsilon) const
{
	return typeid(*this) == typeid(other);
}


#ifndef NDEBUG
void* EstimatorBase::operator new(const size_t count)
{
	++instanceCount;
	return ::operator new(count);
}

void EstimatorBase::operator delete(void* ptr)
{
	--instanceCount;
	return ::operator delete(ptr);
}
#endif
