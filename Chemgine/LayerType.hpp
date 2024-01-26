#pragma once

#include <cstdint>

enum class LayerType : uint8_t
{
	GASEOUS,
	POLAR,
	NONPOLAR,
	DENSE_NONPOLAR,
	MOLTEN_SOLID,
	SOLID,

	REAL_LAYER_COUNT, // marks the number of real layers

	UNKNOWN,
	NONE
};

static inline constexpr uint8_t toIndex(const LayerType type) { return static_cast<uint8_t>(type); }

const uint8_t REAL_LAYER_COUNT = toIndex(LayerType::REAL_LAYER_COUNT);

static inline constexpr bool isRealLayer(const LayerType type) { return toIndex(type) < REAL_LAYER_COUNT; }

static LayerType& operator++(LayerType& layer) 
{
	return layer = static_cast<LayerType>(static_cast<uint8_t>(layer) + 1);
}

static LayerType& operator--(LayerType& layer)
{
	return layer = static_cast<LayerType>(static_cast<uint8_t>(layer) - 1);
}

static LayerType operator+(LayerType& layer, const uint8_t x)
{
	return static_cast<LayerType>(static_cast<uint8_t>(layer) + x);
}

static LayerType operator-(LayerType& layer, const uint8_t x)
{
	return static_cast<LayerType>(static_cast<uint8_t>(layer) - x);
}

static LayerType& operator+=(LayerType& layer, const uint8_t x)
{
	return layer = static_cast<LayerType>(static_cast<uint8_t>(layer) + x);
}

static LayerType& operator-=(LayerType& layer, const uint8_t x)
{
	return layer = static_cast<LayerType>(static_cast<uint8_t>(layer) - x);
}