#pragma once

#include "BaseLabwareData.hpp"
#include "LabwareJoint.hpp"

#include <vector>

class JointedLabwareData : public BaseLabwareData
{
public:
	const std::vector<LabwareJoint> joints;

	JointedLabwareData(
		const LabwareIdType id,
		const std::string& name,
		std::vector<LabwareJoint>&& joints,
		const LabwareType type
	) noexcept;

};