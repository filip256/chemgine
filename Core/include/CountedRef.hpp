#pragma once

#include "Log.hpp"
#include "Casts.hpp"

#include <type_traits>

template<typename CountT>
class Countable;

template <typename T>
concept CountableType = requires(T t)
{
	// requires T derived from Countable<?>
	[] <typename X>(const Countable<X>&) {}(t);
};


template<CountableType T>
class CountedRef;

template<typename CountT = size_t>
class Countable
{
	static_assert(std::is_integral_v<CountT>, "Countable: CountT must be an integral type.");

private:
	mutable CountT count = 0;

public:
	Countable() = default;
	Countable(const Countable&) = default;
	Countable(Countable&&) = default;
    virtual ~Countable();

	CountT getRefCount() const;

	template<CountableType T>
	friend class CountedRef;
};

template<typename CountT>
Countable<CountT>::~Countable()
{
	if (count != 0)
		Log(this).fatal("A CountedRef's lifetime exceeded the lifetime of the referenced object.");
}

template<typename CountT>
CountT Countable<CountT>::getRefCount() const
{
	return count;
}


template<CountableType T>
class CountedRef final
{
private:
	T& object;

public:
	CountedRef(T& object) noexcept;

	CountedRef(CountedRef& other) noexcept;
	template<CountableType D>
	CountedRef(const CountedRef<D>& other) noexcept;

	CountedRef(CountedRef&& other) noexcept;

	~CountedRef() noexcept;

	inline T& operator*();
	inline const T& operator*() const;

	inline T* operator->();
	inline const T* operator->() const;
	template<typename R, typename... Args>
	R operator->*(R(T::* memberFunction)(Args...));
	template<typename R, typename... Args>
	R operator->*(R(T::* memberFunction)(Args...) const) const;

	template<CountableType D>
	bool is() const;
	template<CountableType DstT>
	DstT* cast();
	template<CountableType DstT>
	const DstT* cast() const;
};


template<CountableType T>
CountedRef<T>::CountedRef(T& object) noexcept :
	object(object)
{
	++object.count;
}

template<CountableType T>
CountedRef<T>::CountedRef(CountedRef&& other) noexcept :
	CountedRef(other) // move also increments the ref count since (dtors are called on moved objects too)
{}

template<CountableType T>
template<CountableType D>
CountedRef<T>::CountedRef(const CountedRef<D>& other) noexcept :
	CountedRef<T>(static_cast<T&>(*other))
{}

template<CountableType T>
CountedRef<T>::CountedRef(CountedRef& other) noexcept :
	CountedRef(other.object)
{}

template<CountableType T>
CountedRef<T>::~CountedRef() noexcept
{
	--object.count;
}

template<CountableType T>
T& CountedRef<T>::operator*()
{
	return object;
}

template<CountableType T>
const T& CountedRef<T>::operator*() const
{
	return object;
}

template<CountableType T>
T* CountedRef<T>::operator->()
{
	return &object;
}

template<CountableType T>
const T* CountedRef<T>::operator->() const
{
	return &object;
}

template<CountableType T>
template<typename R, typename... Args>
R CountedRef<T>::operator->*(R(T::* memberFunction)(Args...))
{
	return (object->*memberFunction)();
}

template<CountableType T>
template<typename R, typename... Args>
R CountedRef<T>::operator->*(R(T::* memberFunction)(Args...) const) const
{
	return (object->*memberFunction)();
}

template<CountableType T>
template<CountableType D>
bool CountedRef<T>::is() const
{
	return final_is<D>(object);
}

template<CountableType T>
template<CountableType DstT>
const DstT* CountedRef<T>::cast() const
{
	return final_cast<const DstT>(object);
}

template<CountableType T>
template<CountableType DstT>
DstT* CountedRef<T>::cast()
{
	return final_cast<DstT>(object);
}
