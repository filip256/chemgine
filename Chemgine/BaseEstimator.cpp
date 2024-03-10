#include "BaseEstimator.hpp"
#include "Logger.hpp"

#include <cassert>

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
	Logger::fatal("Undefined estimator method: double get(double)");
	return std::numeric_limits<double>::max();
}

double BaseEstimator::get(const double input1, const double input2) const
{
	Logger::fatal("Undefined estimator method: double get(double, double)");
	return std::numeric_limits<double>::max();
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