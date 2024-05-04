#pragma once

#include "LabwareType.hpp"
#include "LabwareConnectionData.hpp"

#include <cstdint>
#include <string>
#include <vector>

typedef uint32_t LabwareId;

class BaseLabwareData
{
public:
	const LabwareId id;
	const LabwareType type;
	const std::string name;
	const std::vector<LabwarePortData> ports;
	const std::vector<LabwareContactData> contacts;

	BaseLabwareData(
		const LabwareId id,
		const std::string& name,
		std::vector<LabwarePortData>&& ports,
		std::vector<LabwareContactData>&& contacts,
		const LabwareType type
	) noexcept;
	BaseLabwareData(const BaseLabwareData&) = delete;
	BaseLabwareData(BaseLabwareData&&) = default;
	virtual ~BaseLabwareData() = default;

	// for memory leak checking 
	static size_t instanceCount;
#ifndef NDEBUG
	void* operator new(const size_t count);
	void operator delete(void* ptr);
#endif
};
