#pragma once

#include <cstdint>
#include <string>

enum class AggregationType : uint8_t
{
	GAS,
	LIQUID,
	SOLID,

	NONE
};

std::string getLayerName(const AggregationType type);
