#pragma once

#include "FlaskData.hpp"

class JFlaskData : public FlaskData
{
public:
	const uint8_t jointCount;

	JFlaskData(
		const LabwareIdType id,
		const std::string& name,
		const double volume,
		const uint8_t jointCount
	) noexcept;
};