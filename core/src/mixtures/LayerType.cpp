#include "mixtures/LayerType.hpp"

AggregationType getAggregationType(const LayerType type)
{
	return
		isGasLayer(type) ? AggregationType::GAS :
		isLiquidLayer(type) ? AggregationType::LIQUID :
		isSolidLayer(type) ? AggregationType::SOLID :
		AggregationType::NONE;
}

LayerType getLowerAggregationLayer(const LayerType type)
{
	if (isLiquidLayer(type))
		return LayerType::SOLID;
	if (isGasLayer(type))
		return LayerType::POLAR;
	return LayerType::NONE;
}

LayerType getHigherAggregationLayer(const LayerType type)
{
	if (isLiquidLayer(type))
		return LayerType::GASEOUS;
	if (isSolidLayer(type))
		return LayerType::POLAR;
	return LayerType::NONE;
}

LayerType getLayerType(const AggregationType type,
	const bool isNonpolar, const bool isDense)
{
	return
		type == AggregationType::GAS ? LayerType::GASEOUS :
		type == AggregationType::SOLID ? LayerType::SOLID :
		type == AggregationType::LIQUID ? (
			isNonpolar ? (isDense ? LayerType::DENSE_NONPOLAR : LayerType::NONPOLAR) :
			LayerType::POLAR) :
		LayerType::NONE;
}

std::string getLayerName(const LayerType type)
{
	switch (type)
	{
	case LayerType::GASEOUS:
		return "gaseous";
	case LayerType::INORG_LIQUEFIED_GAS:
		return "inorganic liquefied gas";
	case LayerType::NONPOLAR:
		return "nonpolar";
	case LayerType::POLAR:
		return "polar";
	case LayerType::DENSE_NONPOLAR:
		return "dense nonpolar";
	case LayerType::INORG_MOLTEN_SOLID:
		return "inorganic molten solid";
	case LayerType::SOLID:
		return "solid";
	default:
		return "?";
	}
}

LayerType& operator++(LayerType& layer)
{
	return layer = static_cast<LayerType>(static_cast<uint8_t>(layer) << 1);
}

LayerType& operator--(LayerType& layer)
{
	return layer = static_cast<LayerType>(static_cast<uint8_t>(layer) >> 1);
}
