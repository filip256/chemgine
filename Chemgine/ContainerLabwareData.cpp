#include "ContainerLabwareData.hpp"

ContainerLabwareData::ContainerLabwareData(
	const LabwareIdType id,
	const std::string& name,
	const std::string& textureFile,
	const Amount<Unit::LITER> volume,
	std::vector<LabwareJoint>&& joints,
	const LabwareType type
) noexcept :
	DrawableLabwareData(id, name, textureFile, type),
	volume(volume),
	joints(joints)
{}