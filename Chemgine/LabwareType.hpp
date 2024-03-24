#pragma once

#include <cstdint>

enum class LabwareType : uint8_t
{
	NONE = 0,
	FLASK = 1,
	ADAPTOR = 2,
	CONDENSER = 3,
	FUNNEL = 4,
	SEP_FUNNEL = 5,
	DROP_FUNNEL = 6,
	HEATSOURCE = 7,
	STIRRE = 8,
	HEATER_STIRRER = 9
};

static inline constexpr bool hasMultiLayerStorage(const LabwareType type)
{
	return 
		type == LabwareType::FLASK ||
		type == LabwareType::SEP_FUNNEL ||
		type == LabwareType::DROP_FUNNEL;
}
