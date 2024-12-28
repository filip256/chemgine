#pragma once

#include <typeindex>

class TypeId
{
private:
	std::type_index id;

	TypeId(const std::type_index id) noexcept;

public:
	TypeId(const TypeId&) = default;

	template<typename T>
	static TypeId of();

	template<typename T>
	bool is();

	size_t getHashCode() const;
	const char* getTypeName() const;
	std::type_index getTypeIndex() const;

	TypeId& operator=(const TypeId&) = default;

	bool operator==(const TypeId other) const;
	bool operator!=(const TypeId other) const;
};

template<>
struct std::hash<TypeId> {
	size_t operator()(const TypeId& unit) const
	{
		return std::hash<std::type_index>()(unit.getTypeIndex());
	}
};

template<typename T>
TypeId TypeId::of()
{
	return TypeId(typeid(T));
}

template<typename T>
bool TypeId::is()
{
	return *this == TypeId::of<T>();
}
