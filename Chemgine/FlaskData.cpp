#include "FlaskData.hpp"

FlaskData::FlaskData(
	const LabwareIdType id,
	const std::string& name,
	const Amount<Unit::LITER> volume,
	const LabwareType type
) noexcept :
	BaseLabwareData(id, name, type),
	volume(volume)
{}

FlaskData::FlaskData(
	const LabwareIdType id,
	const std::string& name,
	const Amount<Unit::LITER> volume
) noexcept :
	FlaskData(id, name, volume, LabwareType::FLASK)
{}