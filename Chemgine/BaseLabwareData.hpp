#pragma once

#include <cstdint>
#include <string>

#include "LabwareType.hpp"

typedef uint32_t LabwareIdType;

class BaseLabwareData
{
public:
	const LabwareIdType id;
	const LabwareType type;
	const std::string name;

	BaseLabwareData(
		const LabwareIdType id,
		const LabwareType type,
		const std::string& name
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