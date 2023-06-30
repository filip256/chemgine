#pragma once

#include "Value.hpp"

class FloatValue : Value<float>
{
public:
	constexpr FloatValue(float value);
};