#pragma once

#include "BaseLabwareData.hpp"

class FlaskData : public BaseLabwareData
{
public:
	const double volume;

	FlaskData(const LabwareIdType id, const std::string& name, const double volume) noexcept;
};