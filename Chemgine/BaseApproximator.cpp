#include "BaseApproximator.hpp"

#include <cassert>

size_t BaseApproximator::instanceCount = 0;

BaseApproximator::BaseApproximator(const ApproximatorIdType id) noexcept :
	id(id)
{}

ApproximatorIdType BaseApproximator::getId() const
{
	return id;
}

double BaseApproximator::get(const double input) const
{
	assert(("Undefined approximator method: get(double)", true));
	return std::numeric_limits<double>::max();
}

double BaseApproximator::get(const double input1, const double input2) const
{
	assert(("Undefined approximator method: get(double, double).", true));
	return std::numeric_limits<double>::max();
}

#ifndef NDEBUG
void* BaseApproximator::operator new(const size_t count)
{
	++instanceCount;
	return ::operator new(count);
}

void BaseApproximator::operator delete(void* ptr)
{
	--instanceCount;
	return ::operator delete(ptr);
}
#endif