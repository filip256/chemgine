#pragma once

#include "BaseLabwareData.hpp"
#include "FlaskData.hpp"
#include "AdaptorData.hpp"

#include <vector>
#include <string>

class LabwareDataFactory
{
	static FlaskData* getFlask(const LabwareId id, const std::vector<std::string>& dataLine);
	static AdaptorData* getAdaptor(const LabwareId id, const std::vector<std::string>& dataLine);

public:

	/// <summary>
	/// Allocates the right type of LabwareData and returns a pointer to it or nullptr if the operation fails.
	/// LabwareDataFactory does not own the LabwareDatas created in this manner.
	/// </summary>
	static BaseLabwareData* get(const LabwareId id, const LabwareType type, const std::vector<std::string>& dataLine);
};