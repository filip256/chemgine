#include "AdaptorData.hpp"

AdaptorData::AdaptorData(
	const LabwareId id,
	const std::string& name,
	std::vector<LabwarePortData>&& ports,
	const Amount<Unit::LITER> volume,
	const std::string& textureFile,
	const float textureScale
) noexcept :
	ContainerLabwareData(
		id, name,
		std::move(ports), std::vector<LabwareContactData>(), 
		textureFile, textureScale,
		volume,
		LabwareType::ADAPTOR)
{}
