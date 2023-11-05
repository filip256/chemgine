#pragma once

template<class T>
class Value
{
protected:
	T value;

public:
	static const T epsilon;

	constexpr Value(const T value) noexcept;

	Value<T>& operator=(const Value<T>& other);

	Value<T> operator+(const Value<T>& other) const;
	Value<T> operator-(const Value<T>& other) const;
	Value<T> operator*(const T scalar) const;
	Value<T> operator/(const T divisor) const;

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
};