#include "Value.hpp"

#include <cmath>
#include <limits>

template<class T>
const T Value<T>::epsilon = std::numeric_limits<T>::epsilon();

template<class T>
constexpr Value<T>::Value(const T value) noexcept :
	value(value)
{}

template<class T>
Value<T>& Value<T>::operator=(const Value<T>& other)
{
	if (this != &other) 
		value = other.value;

	return *this;
}

template <class T>
bool Value<T>::operator==(const Value<T>& other) const { return std::abs(value - other.value) <= epsilon; }

template <class T>
bool Value<T>::operator!=(const Value<T>& other) const { return !(*this == other); }

template <class T>
bool Value<T>::operator<(const Value<T>& other) const { return value < other.value; }

template <class T>
bool Value<T>::operator<=(const Value<T>& other) const { return *this < other || *this == other; }

template <class T>
bool Value<T>::operator>(const Value<T>& other) const { return value > other.value; }

template <class T>
bool Value<T>::operator>=(const Value<T>& other) const { return *this > other || *this == other;; }

template <class T>
Value<T> Value<T>::operator+(const Value<T>& other) const { return Value<T>(value + other.value); }

template <class T>
Value<T> Value<T>::operator-(const Value<T>& other) const { return Value<T>(value - other.value); }

template <class T>
Value<T> Value<T>::operator*(const T scalar) const { return Value<T>(value * scalar); }

template <class T>
Value<T> Value<T>::operator/(const T divisor) const { return Value<T>(value / divisor); }

template <class T>
Value<T> Value<T>::operator-() const { return Value<T>(-1 * value); }

template <class T>
Value<T>& Value<T>::operator+=(const Value<T>& other)
{
	value += other.value;
	return *this;
}

template <class T>
Value<T>& Value<T>::operator-=(const Value<T>& other)
{
	value -= other.value;
	return *this;
}

template <class T>
Value<T>& Value<T>::operator*=(const T scalar)
{
	value *= scalar;
	return *this;
}

template <class T>
Value<T>& Value<T>::operator/=(const T divisor)
{
	value /= divisor;
	return *this;
}

template class Value<float>;
template class Value<double>;
template class Value<long double>;