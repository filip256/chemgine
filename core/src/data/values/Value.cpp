#include "data/values/Value.hpp"

#include "global/Precision.hpp"
#include "utils/Numeric.hpp"

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
bool Value<T>::operator==(const Value<T>& other) const { return utils::floatEqual(value, other.value, epsilon); }

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

template <class T>
constexpr bool Value<T>::oveflowsOnAdd(const Value<T>& other) const noexcept
{
	return
		this->value > 0.0 && other.value > 0.0 ?
		this->value > std::numeric_limits<T>::max() - other.value :
	this->value < 0.0 && other.value < 0.0 ?
		this->value < std::numeric_limits<T>::min() - other.value :
		false;
}

template <class T>
constexpr bool Value<T>::oveflowsOnMultiply(const Value<T>& other) const noexcept
{
	return
		this->value > 0.0 ? (
			other.value > 0.0 ?
			this->value > std::numeric_limits<T>::max() / other.value :
			this->value > std::numeric_limits<T>::min() / other.value
		) : (
		other.value > 0.0 ?
			this->value < std::numeric_limits<T>::min() / other.value :
			this->value < std::numeric_limits<T>::max() / other.value
		);
}


template class Value<float_s>;
