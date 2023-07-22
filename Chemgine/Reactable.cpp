#include "Reactable.hpp"

size_t Reactable::instanceCount = 0;
DataStoreAccessor Reactable::dataAccessor = DataStoreAccessor();

Reactable::Reactable() noexcept
{
	dataAccessor.crashIfUninitialized();
}

void Reactable::setDataStore(const DataStore& dataStore)
{
	dataAccessor.set(dataStore);
}

const DataStore& Reactable::dataStore() const
{
	return dataAccessor.get();
}

#ifndef NDEBUG
void* Reactable::operator new(const size_t count)
{
	++instanceCount;
	return ::operator new(count);
}

void Reactable::operator delete(void* ptr)
{
	--instanceCount;
	return ::operator delete(ptr);
}
#endif