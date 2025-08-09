#include "mixtures/AggregationType.hpp"

std::string getLayerName(const AggregationType type)
{
	switch (type)
	{
	case AggregationType::GAS:
		return "gas";
	case AggregationType::LIQUID:
		return "liquid";
	case AggregationType::SOLID:
		return "solid";
	default:
		return "?";
	}
}