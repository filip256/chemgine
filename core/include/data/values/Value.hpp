#pragma once

#include <limits>

template<typename T>
class Value
{
protected:
	T value = 0.0;

public:
	static const T epsilon;

	constexpr Value() = default;
	constexpr Value(const T value) noexcept;

	Value<T>& operator=(const Value<T>& other);

	Value<T> operator+(const Value<T>& other) const;
	Value<T> operator-(const Value<T>& other) const;
	Value<T> operator*(const T scalar) const;
	Value<T> operator/(const T divisor) const;
	Value<T> operator-() const;

	Value<T>& operator+=(const Value<T>& other);
	Value<T>& operator-=(const Value<T>& other);
	Value<T>& operator*=(const T scalar);
	Value<T>& operator/=(const T divisor);

	bool operator==(const Value<T>& other) const;
	bool operator!=(const Value<T>& other) const;
	bool operator<(const Value<T>& other) const;
	bool operator<=(const Value<T>& other) const;
	bool operator>(const Value<T>& other) const;
	bool operator>=(const Value<T>& other) const;

	constexpr bool overflowsOnAdd(const Value<T>& other) const noexcept;
	constexpr bool overflowsOnMultiply(const Value<T>& other) const noexcept;
};

template<typename T>
constexpr Value<T>::Value(const T value) noexcept :
	value(value)
{}

template <typename T>
constexpr bool Value<T>::overflowsOnAdd(const Value<T>& other) const noexcept
{
	return
		this->value > 0.0 && other.value > 0.0 ?
		this->value > std::numeric_limits<T>::max() - other.value :
	this->value < 0.0 && other.value < 0.0 ?
		this->value < std::numeric_limits<T>::min() - other.value :
		false;
}

template <typename T>
constexpr bool Value<T>::overflowsOnMultiply(const Value<T>& other) const noexcept
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
