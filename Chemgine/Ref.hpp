#pragma once

template<typename T>
class Ref
{
private:
	T* object = nullptr;

	Ref() = default;

public:
	Ref(T& object) noexcept;
	Ref(const Ref&) = default;

	template<typename D, typename = 
		std::enable_if_t<std::is_convertible_v<D&, T&>>>
	Ref(const Ref<D>& other) noexcept;

	bool isSet() const;
	void set(T& object);
	void unset();

	inline T& get();
	inline const T& get() const;
	inline T* operator->();
	inline const T* operator->() const;

	bool operator==(const Ref<T>& other) const;
	bool operator!=(const Ref<T>& other) const;

	const static Ref<T> nullRef;

	template<typename U>
	friend class Ref;
};

template<typename T>
const Ref<T> Ref<T>::nullRef = Ref<T>();

template<typename T>
Ref<T>::Ref(T& object) noexcept:
	object(&object)
{}

template<typename T>
template<typename D, typename>
Ref<T>::Ref(const Ref<D>& other) noexcept:
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
bool Ref<T>::operator==(const Ref<T>& other) const
{
	return &(this->object) == &(other.object);
}

template<typename T>
bool Ref<T>::operator!=(const Ref<T>& other) const
{
	return &(this->object) != &(other.object);;
}