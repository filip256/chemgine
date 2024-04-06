#pragma once

class NullRefType
{
public:
	explicit constexpr NullRefType (int) {}
};

constexpr NullRefType nullRef(0);


template<typename T>
class Ref
{
private:
	T* object = nullptr;

	Ref() = default;
	Ref(T* object) noexcept;

public:
	Ref(T& object) noexcept;
	Ref(NullRefType) noexcept;
	Ref(const Ref&) = default;

	template<typename D, typename = 
		std::enable_if_t<std::is_convertible_v<D&, T&>>>
	Ref(const Ref<D>& other) noexcept;

	bool isSet() const;
	void set(T& object);
	void unset();

	template<typename D>
	D& as() const;
	template<typename D>
	Ref<D> cast() const;

	Ref<T>& operator=(Ref<T> other);

	inline T& get();
	inline const T& get() const;
	inline T& operator*();
	inline const T& operator*() const;
	inline T* operator->();
	inline const T* operator->() const;

	template<typename R, typename... Args>
	R operator->*(R(T::* memberFunction)(Args...));
	template<typename R, typename... Args>
	R operator->*(R(T::* memberFunction)(Args...) const) const;

	bool operator==(const Ref<T>& other) const;
	bool operator!=(const Ref<T>& other) const;

	explicit operator bool() const noexcept
	{
		return object != nullptr;
	}

	template<typename U>
	friend class Ref;
};


template<typename T>
Ref<T>::Ref(T* object) noexcept :
	object(object)
{}

template<typename T>
Ref<T>::Ref(T& object) noexcept :
	object(&object)
{}

template<typename T>
Ref<T>::Ref(NullRefType) noexcept :
	object(nullptr)
{}

template<typename T>
template<typename D, typename>
Ref<T>::Ref(const Ref<D>& other) noexcept :
	Ref<T>(static_cast<T&>(*other.object))
{}

template<typename T>
bool Ref<T>::isSet() const 
{
	return object != nullptr;
}

template<typename T>
void Ref<T>::set(T& object) 
{
	this->object = &object;
}

template<typename T>
void Ref<T>::unset() 
{
	object = nullptr;
}

template<typename T>
template<typename D>
D& Ref<T>::as() const
{
	return static_cast<D&>(*object);
}

template<typename T>
template<typename D>
Ref<D> Ref<T>::cast() const
{
	return Ref<D>(dynamic_cast<D*>(object));
}

template<typename T>
Ref<T>& Ref<T>::operator=(Ref<T> other)
{
	this->object = other.object;
	return *this;
}

template<typename T>
T& Ref<T>::get()
{
	return *object;
}

template<typename T>
const T& Ref<T>::get() const
{
	return *object;
}

template<typename T>
T& Ref<T>::operator*()
{
	return *object;
}

template<typename T>
const T& Ref<T>::operator*() const
{
	return *object;
}

template<typename T>
T* Ref<T>::operator->() 
{
	return object;
}

template<typename T>
const T* Ref<T>::operator->() const 
{
	return object;
}

template<typename T>
template<typename R, typename... Args>
R Ref<T>::operator->*(R(T::* memberFunction)(Args...)) 
{
	return (object->*memberFunction)();
}

template<typename T>
template<typename R, typename... Args>
R Ref<T>::operator->*(R(T::* memberFunction)(Args...) const) const
{
	return (object->*memberFunction)();
}

template<typename T>
bool Ref<T>::operator==(const Ref<T>& other) const
{
	return &(this->object) == &(other.object);
}

template<typename T>
bool Ref<T>::operator!=(const Ref<T>& other) const
{
	return &(this->object) != &(other.object);;
}
