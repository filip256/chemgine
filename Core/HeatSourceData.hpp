#pragma once

#include "DrawableLabwareData.hpp"
#include "Amount.hpp"

class HeatsourceData : public DrawableLabwareData
{
public:
	const Amount<Unit::WATT> maxPowerOutput;

	HeatsourceData(
		const LabwareId id,
		const std::string& name,
		std::vector<LabwarePort>&& ports,
		const Amount<Unit::WATT> maxPowerOutput,
		const std::string& textureFile, 
		const float_s textureScale
	) noexcept;

	void dumpCustomProperties(Def::DataDumper& dump) const override final;
};
