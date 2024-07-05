#include "BaseEstimator.hpp"
#include "Log.hpp"

#include <cassert>
#include <typeinfo>

size_t BaseEstimator::instanceCount = 0;

BaseEstimator::BaseEstimator(const EstimatorId id) noexcept :
	id(id)
{}

EstimatorId BaseEstimator::getId() const
{
	return id;
}

double BaseEstimator::get(const double input) const
{
	Log(this).fatal("Undefined estimator method: double get(double)");
	return std::numeric_limits<double>::max();
}

double BaseEstimator::get(const double input1, const double input2) const
{
	Log(this).fatal("Undefined estimator method: double get(double, double)");
	return std::numeric_limits<double>::max();
}

bool BaseEstimator::isEquivalent(const BaseEstimator& other, const double epsilon) const
{
	return typeid(*this) == typeid(other);
}


#ifndef NDEBUG
void* BaseEstimator::operator new(const size_t count)
{
	++instanceCount;
	return ::operator new(count);
}

void BaseEstimator::operator delete(void* ptr)
{
	--instanceCount;
	return ::operator delete(ptr);
}
#endif
