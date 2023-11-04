#include "JointedLabwareData.hpp"

JointedLabwareData::JointedLabwareData(
	const LabwareIdType id,
	const std::string& name,
	std::vector<LabwareJoint>&& joints,
	const LabwareType type
) noexcept :
	BaseLabwareData(id, name, type),
	joints(std::move(joints))
{}