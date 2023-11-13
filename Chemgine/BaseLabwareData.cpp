#include "BaseLabwareData.hpp"

size_t BaseLabwareData::instanceCount = 0;

BaseLabwareData::BaseLabwareData(
	const LabwareIdType id,
	const std::string& name,
	std::vector<LabwarePort>&& ports,
	const LabwareType type
) noexcept :
	id(id),
	name(name),
	ports(std::move(ports)),
	type(type)
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