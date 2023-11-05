#pragma once

#include "BaseLabwareData.hpp"
#include "FlaskData.hpp"
#include "AdaptorData.hpp"

#include <vector>
#include <string>

class LabwareDataFactory
{
	static FlaskData* getFlask(const LabwareIdType id, const std::vector<std::string>& dataLine);
	static AdaptorData* getAdaptor(const LabwareIdType id, const std::vector<std::string>& dataLine);

public:

	/// <summary>
	/// Allocates the right type of LabwareData and returns a pointer to it or nullptr if the operation fails.
	/// LabwareDataFactory does not own the LabwareDatas created in this manner.
	/// </summary>
	static BaseLabwareData* get(const LabwareIdType id, const LabwareType type, const std::vector<std::string>& dataLine);
};