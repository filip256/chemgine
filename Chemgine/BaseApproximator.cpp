#include "BaseApproximator.hpp"

size_t BaseApproximator::instanceCount = 0;

BaseApproximator::BaseApproximator(
	const ApproximatorIdType id,
	const std::string& name,
	const ApproximatorType type
) noexcept :
	id(id),
	name(name),
	type(type)
{}

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