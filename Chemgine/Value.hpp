#pragma once

template<class T>
class Value
{
public:
	T value;

	constexpr Value(T value);

	constexpr T asKilo() const;
	constexpr T asStd() const;
	constexpr T asMilli() const;
	constexpr T asMicro() const;
};