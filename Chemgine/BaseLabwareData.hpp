#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <ostream>

#include "LabwareType.hpp"
#include "LabwarePort.hpp"

typedef uint32_t LabwareId;

class BaseLabwareData
{
public:
	const LabwareId id;
	const LabwareType type;
	const std::string name;
	const std::vector<LabwarePort> ports;

	BaseLabwareData(
		const LabwareId id,
		const std::string& name,
		std::vector<LabwarePort>&& ports,
		const LabwareType type
	) noexcept;
	BaseLabwareData(const BaseLabwareData&) = delete;
	BaseLabwareData(BaseLabwareData&&) = default;
	virtual ~BaseLabwareData() = default;

	virtual void printDefinition(std::ostream& out) const = 0;
	virtual void dumpTextures(const std::string& path) const = 0;
};
