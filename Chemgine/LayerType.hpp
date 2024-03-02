#pragma once

#include "AggregationType.hpp"

enum class LayerType : uint8_t
{
	GASEOUS,
	INORG_LIQUEFIED_GAS,
	NONPOLAR,
	POLAR,
	DENSE_NONPOLAR,
	INORG_MOLTEN_SOLID,
	SOLID,

	REAL_LAYER_COUNT, // marks the number of real layers

	FIRST = GASEOUS,
	LAST = SOLID,

	UNKNOWN,
	NONE
};



static inline constexpr uint8_t toIndex(const LayerType type)
{
	return static_cast<uint8_t>(type);
}

static inline constexpr bool isRealLayer(const LayerType type)
{
	return toIndex(type) < toIndex(LayerType::REAL_LAYER_COUNT);
}

static inline constexpr bool isGasLayer(const LayerType type)
{
	return type == LayerType::GASEOUS;
}

static inline constexpr bool isLiquidLayer(const LayerType type)
{
	return type >= LayerType::INORG_LIQUEFIED_GAS && type <= LayerType::INORG_MOLTEN_SOLID;
}

static inline constexpr bool isSolidLayer(const LayerType type)
{
	return type == LayerType::SOLID;
}

AggregationType getAggregationType(const LayerType type);
LayerType getLowerAggregationLayer(const LayerType type);
LayerType getHigherAggregationLayer(const LayerType type);
LayerType getLayerType(const AggregationType type,
	const bool isNonpolar, const bool isDense);

std::string getLayerName(const LayerType type);

LayerType& operator++(LayerType& layer);
LayerType& operator--(LayerType& layer);
LayerType operator+(LayerType& layer, const uint8_t x);
LayerType operator-(LayerType& layer, const uint8_t x);
LayerType& operator+=(LayerType& layer, const uint8_t x);
LayerType& operator-=(LayerType& layer, const uint8_t x);
