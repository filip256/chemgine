#include "JFlaskData.hpp"

JFlaskData::JFlaskData(
	const LabwareIdType id,
	const std::string& name,
	const double volume,
	const uint8_t jointCount
) noexcept :
	FlaskData(id, name, volume, LabwareType::J_FLASK),
	jointCount(jointCount)
{}