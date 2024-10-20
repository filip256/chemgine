#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <ostream>

#include "LabwareType.hpp"
#include "LabwarePort.hpp"
#include "DataDumper.hpp"

typedef uint32_t LabwareId;

class BaseLabwareData
{
private:
	virtual void dumpCustomProperties(DataDumper& dump) const = 0;

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

	void dumpDefinition(std::ostream& out, const bool prettify) const;
	virtual void dumpTextures(const std::string& path) const = 0;
	void print(std::ostream& out = std::cout) const;
};
