#pragma once

#include <cstdint>

enum class LabwareType : uint8_t
{
	NONE,
	FLASK,
	ADAPTOR,
	CONDENSER,
	FUNNEL,
	SEP_FUNNEL,
	DROP_FUNNEL,
	HEATER,
	STIRRER,
	HEATER_STIRRER
};

static inline constexpr bool hasMultiLayerStorage(const LabwareType type)
{
	return 
		type == LabwareType::FLASK ||
		type == LabwareType::SEP_FUNNEL ||
		type == LabwareType::DROP_FUNNEL;
}
