#pragma once

#include "BaseLabwareData.hpp"
#include "Amount.hpp"

class FlaskData : public BaseLabwareData
{
protected:
	FlaskData(
		const LabwareIdType id,
		const std::string& name,
		const Amount<Unit::LITER> volume,
		const LabwareType type
	) noexcept;

public:
	const Amount<Unit::LITER> volume;

	FlaskData(
		const LabwareIdType id,
		const std::string& name,
		const Amount<Unit::LITER> volume
	) noexcept;
};