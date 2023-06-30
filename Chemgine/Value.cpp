#include "Value.hpp"


template<class T>
constexpr Value<T>::Value(T value) :
	value(value)
{}

template<class T>
constexpr T Value<T>::asKilo() const { return value / 1000.0; }

template<class T>
constexpr T Value<T>::asStd() const { return value; }

template<class T>
constexpr T Value<T>::asMilli() const { return value * 1000.0; }

template<class T>
constexpr T Value<T>::asMicro() const { return value * 1000000.0; }