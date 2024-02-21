#pragma once

#include "AggregationType.hpp"

enum class LayerType : uint8_t
{
	GASEOUS,
	LIQUEFIED_GAS,
	NONPOLAR,
	POLAR,
	DENSE_NONPOLAR,
	MOLTEN_SOLID,
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
	return type >= LayerType::LIQUEFIED_GAS && type <= LayerType::MOLTEN_SOLID;
}

static inline constexpr bool isSolidLayer(const LayerType type)
{
	return type == LayerType::GASEOUS;
}

inline AggregationType getAggregation(const LayerType type)
{
	return
		isGasLayer(type) ? AggregationType::GAS :
		isSolidLayer(type) ? AggregationType::SOLID :
		AggregationType::LIQUID;
}

inline LayerType getLowerAggregationLayer(const LayerType type)
{
	if (isLiquidLayer(type))
		return LayerType::SOLID;
	if (isGasLayer(type))
		return LayerType::POLAR;
	return LayerType::NONE;
}

inline LayerType getHigherAggregationLayer(const LayerType type)
{
	if (isLiquidLayer(type))
		return LayerType::GASEOUS;
	if (isSolidLayer(type))
		return LayerType::POLAR;
	return LayerType::NONE;
}

inline LayerType getLayer(const AggregationType type)
{
	return
		type == AggregationType::GAS ? LayerType::GASEOUS :
		type == AggregationType::SOLID ? LayerType::SOLID :
		LayerType::POLAR; // TODO: add polarity logic
}

static LayerType& operator++(LayerType& layer)
{
	return layer = static_cast<LayerType>(toIndex(layer) + 1);
}

static LayerType& operator--(LayerType& layer)
{
	return layer = static_cast<LayerType>(toIndex(layer) - 1);
}

static LayerType operator+(LayerType& layer, const uint8_t x)
{
	return static_cast<LayerType>(toIndex(layer) + x);
}

static LayerType operator-(LayerType& layer, const uint8_t x)
{
	return static_cast<LayerType>(toIndex(layer) - x);
}

static LayerType& operator+=(LayerType& layer, const uint8_t x)
{
	return layer = static_cast<LayerType>(toIndex(layer) + x);
}

static LayerType& operator-=(LayerType& layer, const uint8_t x)
{
	return layer = static_cast<LayerType>(toIndex(layer) - x);
}