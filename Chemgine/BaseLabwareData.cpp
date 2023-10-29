#include "BaseLabwareData.hpp"

size_t BaseLabwareData::instanceCount = 0;

BaseLabwareData::BaseLabwareData(
	const LabwareIdType id,
	const LabwareType type,
	const std::string& name
) noexcept :
	id(id),
	type(type),
	name(name)
{}

#ifndef NDEBUG
void* BaseLabwareData::operator new(const size_t count)
{
	++instanceCount;
	return ::operator new(count);
}

void BaseLabwareData::operator delete(void* ptr)
{
	--instanceCount;
	return ::operator delete(ptr);
}
#endif