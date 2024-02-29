#pragma once

#include <type_traits>

template <class EnumT>
class FlagField
{
	static_assert(std::is_enum<EnumT>::value, "EnumT must be an enumeration type");

private:
	using StorageT = typename std::underlying_type<EnumT>::type;

	StorageT field = 0;

	FlagField(const StorageT field) noexcept;

public:
	FlagField() = default;
	FlagField(const EnumT field) noexcept;

	void set(const FlagField<EnumT> flags);
	void raise(const FlagField<EnumT> flags);
	void remove(const FlagField<EnumT> flags);
	bool has(const FlagField<EnumT> flags) const;

	FlagField<EnumT> operator|(const FlagField<EnumT> other) const;
	FlagField<EnumT> operator&(const FlagField<EnumT> other) const;
	FlagField<EnumT> operator^(const FlagField<EnumT> other) const;
	FlagField<EnumT> operator-(const FlagField<EnumT> other) const;
};

template <class EnumT>
FlagField<EnumT>::FlagField(const StorageT field) noexcept :
	field(field)
{}

template <class EnumT>
FlagField<EnumT>::FlagField(const EnumT field) noexcept :
	field(static_cast<StorageT>(field))
{}

template <class EnumT>
void FlagField<EnumT>::set(const FlagField<EnumT> flags)
{
	field = flags.field;
}

template <class EnumT>
void FlagField<EnumT>::raise(const FlagField<EnumT> flags)
{
	field |= flags.field;
}

template <class EnumT>
void FlagField<EnumT>::remove(const FlagField<EnumT> flags)
{
	field &= ~flags.field;
}

template <class EnumT>
bool FlagField<EnumT>::has(const FlagField<EnumT> flags) const
{
	return (field & flags.field) != 0;
}

template <class EnumT>
FlagField<EnumT> FlagField<EnumT>::operator|(const FlagField<EnumT> other) const 
{
	return this->field | other.field;
}

template <class EnumT>
FlagField<EnumT> FlagField<EnumT>::operator&(const FlagField<EnumT> other) const
{
	return this->field & other.field;
}

template <class EnumT>
FlagField<EnumT> FlagField<EnumT>::operator^(const FlagField<EnumT> other) const
{
	return this->field ^ other.field;
}

template <class EnumT>
FlagField<EnumT> FlagField<EnumT>::operator-(const FlagField<EnumT> other) const
{
	return this->field & ~other.field;
}