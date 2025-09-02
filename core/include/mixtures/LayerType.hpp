#pragma once

#include "mixtures/AggregationType.hpp"
#include "utils/Math.hpp"

enum class LayerType : uint8_t
{
    NONE = 0,

    GASEOUS             = 1 << 0,
    INORG_LIQUEFIED_GAS = 1 << 1,
    NONPOLAR            = 1 << 2,
    POLAR               = 1 << 3,
    DENSE_NONPOLAR      = 1 << 4,
    INORG_MOLTEN_SOLID  = 1 << 5,
    SOLID               = 1 << 6,

    ANY = (1 << 7) - 1,

    FIRST = GASEOUS,
    LAST  = SOLID,
};

static inline uint8_t toIndex(const LayerType type) { return utils::ilog2(static_cast<uint8_t>(type)); }

static inline constexpr bool isRealLayer(const LayerType type)
{
    return type >= LayerType::FIRST && type <= LayerType::LAST;
}

static inline constexpr bool isGasLayer(const LayerType type) { return type == LayerType::GASEOUS; }

static inline constexpr bool isLiquidLayer(const LayerType type)
{
    return type >= LayerType::INORG_LIQUEFIED_GAS && type <= LayerType::INORG_MOLTEN_SOLID;
}

static inline constexpr bool isSolidLayer(const LayerType type) { return type == LayerType::SOLID; }

static inline bool getLayerCount() { return toIndex(LayerType::LAST); }

AggregationType getAggregationType(const LayerType type);
LayerType       getLowerAggregationLayer(const LayerType type);
LayerType       getHigherAggregationLayer(const LayerType type);
LayerType       getLayerType(const AggregationType type, const bool isNonpolar, const bool isDense);

std::string getLayerName(const LayerType type);

LayerType& operator++(LayerType& layer);
LayerType& operator--(LayerType& layer);
