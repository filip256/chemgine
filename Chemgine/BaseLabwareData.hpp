#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "LabwareType.hpp"
#include "LabwarePort.hpp"

typedef uint32_t LabwareIdType;

class BaseLabwareData
{
public:
	const LabwareIdType id;
	const LabwareType type;
	const std::string name;
	const std::vector<LabwarePort> ports;

	BaseLabwareData(
		const LabwareIdType id,
		const std::string& name,
		std::vector<LabwarePort>&& ports,
		const LabwareType type
	) noexcept;
	BaseLabwareData(const BaseLabwareData&) = delete;
	BaseLabwareData(BaseLabwareData&&) = default;

	// for memory leak checking 
	static size_t instanceCount;
#ifndef NDEBUG
	void* operator new(const size_t count);
	void operator delete(void* ptr);
#endif
};