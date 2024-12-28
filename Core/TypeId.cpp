#include "TypeId.hpp"

TypeId::TypeId(const std::type_index id) noexcept :
	id(id)
{}

size_t TypeId::getHashCode() const
{
	return id.hash_code();
}

const char* TypeId::getTypeName() const
{
	return id.name();
}

std::type_index TypeId::getTypeIndex() const
{
	return id;
}

bool TypeId::operator==(const TypeId other) const
{
	return id == other.id;
}

bool TypeId::operator!=(const TypeId other) const
{
	return !(*this == other);
}
