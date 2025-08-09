#pragma once

#include "labware/data/ContainerLabwareData.hpp"

class FlaskData : public ContainerLabwareData<1>
{
public:
	FlaskData(
		const LabwareId id,
		const std::string& name,
		std::vector<LabwarePort>&& ports,
		const Amount<Unit::LITER> volume,
		const std::string& textureFile,
		const float_s textureScale
	) noexcept;

	void dumpCustomProperties(def::DataDumper& dump) const override final;
};
