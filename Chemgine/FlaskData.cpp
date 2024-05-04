#include "FlaskData.hpp"

#include <iostream>

FlaskData::FlaskData(
	const LabwareId id,
	const std::string& name,
	std::vector<LabwarePortData>&& ports,
	std::vector<LabwareContactData>&& contacts,
	const Amount<Unit::LITER> volume,
	const std::string& textureFile,
	const float textureScale
) noexcept :
	ContainerLabwareData(
		id, name,
		std::move(ports), std::move(contacts),
		textureFile, textureScale,
		volume,
		LabwareType::FLASK)
{}
