#pragma once

#include <type_traits>

template <class EnumT>
class FlagField
{
	static_assert(std::is_enum<EnumT>::value, "EnumT must be an enumeration type");

private:
	using StorageT = typename std::underlying_type<EnumT>::type;

	class FlagIterator : public std::iterator<std::forward_iterator_tag, EnumT>
	{
	private:
		StorageT field;
		uint8_t idx = 0;

		FlagIterator(StorageT field) : field(field) {}

	public:
		EnumT operator*() const;

		FlagIterator& operator++();

		bool operator!=(const FlagIterator& other) const;

		static const uint8_t npos = static_cast<uint8_t>(-1);

		friend class FlagIterator;
	};

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

	FlagIterator begin() const;
	FlagIterator end() const;
};


template<class EnumT>
EnumT FlagField<EnumT>::FlagIterator::operator*() const
{
	return static_cast<EnumT>(1 << idx);
}

template<class EnumT>
typename FlagField<EnumT>::FlagIterator& FlagField<EnumT>::FlagIterator::operator++()
{
	++idx;
	while ((field & (1 << idx)) == 0 && idx < sizeof(StorageT) * 8)
		++idx;
	return *this;
}

template<class EnumT>
bool FlagField<EnumT>::FlagIterator::operator!=(const FlagField<EnumT>::FlagIterator& other) const
{
	return idx != other.idx;
}



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
FlagField<EnumT>::FlagIterator FlagField<EnumT>::begin() const
{
	return FlagIterator(field);
}

template <class EnumT>
FlagField<EnumT>::FlagIterator FlagField<EnumT>::end() const
{
	return FlagIterator(FlagIterator::npos);
}
