#pragma once

#include "BaseComponentData.hpp"

class FunctionalGroupData : public BaseComponentData
{
public:
	FunctionalGroupData(
		const ComponentIdType id,
		const std::string& symbol,
		const std::string& name,
		const double weight,
		const uint8_t valence);
};