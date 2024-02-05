#pragma once

#include "BaseContainer.hpp"

size_t BaseContainer::instanceCount = 0;

#ifndef NDEBUG
void* BaseContainer::operator new(const size_t count)
{
	++instanceCount;
	return ::operator new(count);
}

void BaseContainer::operator delete(void* ptr)
{
	--instanceCount;
	return ::operator delete(ptr);
}
#endif