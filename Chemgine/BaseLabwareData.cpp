#include "BaseLabwareData.hpp"

BaseLabwareData::BaseLabwareData(
	const LabwareId id,
	const std::string& name,
	std::vector<LabwarePort>&& ports,
	const LabwareType type
) noexcept :
	id(id),
	name(name),
	ports(std::move(ports)),
	type(type)
{}
