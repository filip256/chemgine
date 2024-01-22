#pragma once

#include <type_traits>

template <class EnumT>
class FlagField
{
	static_assert(std::is_enum<EnumT>::value, "EnumT must be an enumeration type");

private:
	using StorageT = typename std::underlying_type<EnumT>::type;

	StorageT field = 0;

public:
	FlagField() = default;

	void raise(const EnumT flag);
	void remove(const EnumT flag);
	bool has(const EnumT flag) const;
};

template <class EnumT>
void FlagField<EnumT>::raise(const EnumT flag) 
{
	field |= flag;
}

template <class EnumT>
void FlagField<EnumT>::remove(const EnumT flag) 
{
	field &= ~flag;
}

template <class EnumT>
bool FlagField<EnumT>::has(const EnumT flag) const 
{
	return (field & flag) != 0;
}