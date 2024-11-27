#pragma once

#include <type_traits>

template <class EnumT>
class FlagField;

template <class EnumT>
class FlagIterator
{
public:
	using StorageT = typename std::underlying_type<EnumT>::type;

private:
	uint8_t idx;
	StorageT field;

	FlagIterator(const StorageT field, const uint8_t idx = 0) noexcept;

	void findNext();

	static FlagIterator begin(const StorageT field);
	static FlagIterator end(const StorageT field);

public:
	EnumT operator*() const;

	FlagIterator& operator++();

	bool operator!=(const FlagIterator& other) const;

	template <class EnumT>
	friend class FlagField;
};


template <class EnumT>
class FlagField
{
	static_assert(std::is_enum<EnumT>::value, "FlagField: EnumT must be an enumeration type");

public:
	using StorageT = typename std::underlying_type<EnumT>::type;

private:
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

	FlagIterator<EnumT> begin() const;
	FlagIterator<EnumT> end() const;

	static const FlagField None;
	static const FlagField All;
};


template<class EnumT>
FlagIterator<EnumT>::FlagIterator(const StorageT field, const uint8_t idx) noexcept :
	field(field),
	idx(idx)
{
	findNext();
}

template<class EnumT>
void FlagIterator<EnumT>::findNext()
{
	while ((field & (1 << idx)) == 0 && idx < sizeof(StorageT) * 8)
		++idx;
}

template<class EnumT>
EnumT FlagIterator<EnumT>::operator*() const
{
	return static_cast<EnumT>(1 << idx);
}

template<class EnumT>
typename FlagIterator<EnumT>& FlagIterator<EnumT>::operator++()
{
	++idx;
	findNext();
	return *this;
}

template<class EnumT>
bool FlagIterator<EnumT>::operator!=(const FlagIterator& other) const
{
	return idx != other.idx;
}

template<class EnumT>
FlagIterator<EnumT> FlagIterator<EnumT>::begin(const StorageT field)
{
	return FlagIterator<EnumT>(field, 0);
}

template<class EnumT>
FlagIterator<EnumT> FlagIterator<EnumT>::end(const StorageT field)
{
	return FlagIterator<EnumT>(field, sizeof(StorageT) * 8);
}


template <class EnumT>
const FlagField<EnumT> FlagField<EnumT>::None = FlagField(0);

template <class EnumT>
const FlagField<EnumT> FlagField<EnumT>::All = FlagField(std::numeric_limits<StorageT>::max());


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

template <class EnumT>
FlagIterator<EnumT> FlagField<EnumT>::begin() const
{
	return FlagIterator<EnumT>::begin(field);
}

template <class EnumT>
FlagIterator<EnumT> FlagField<EnumT>::end() const
{
	return FlagIterator<EnumT>::end(field);
}
