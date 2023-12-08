#pragma once

#include <cstdint>

enum class PortType : uint8_t
{
	NONE = 0,
	SIMPLE = 1,
	INLET = 2,
	OUTLET = 3,
	DROPPER = 4,
	HOSE = 5
};