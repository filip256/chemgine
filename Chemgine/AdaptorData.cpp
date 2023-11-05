#include "AdaptorData.hpp"

AdaptorData::AdaptorData(
	const LabwareIdType id,
	const std::string& name,
	const Amount<Unit::LITER> volume,
	std::vector<LabwareJoint>&& joints,
	const std::string& textureFile
) noexcept :
	ContainerLabwareData(id, name, textureFile, volume, std::move(joints), LabwareType::ADAPTOR)
{}