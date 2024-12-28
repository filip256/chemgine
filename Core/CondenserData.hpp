#pragma once

#include "ContainerLabwareData.hpp"

class CondenserData : public ContainerLabwareData<2>
{
public:
	const Quantity<Meter> length;
	const Quantity<PerMeter> efficiency;

	CondenserData(
		const LabwareId id,
		const std::string& name,
		std::vector<LabwarePort>&& ports,
		const Quantity<Liter> innerVolume,
		const Quantity<Meter> length,
		const Quantity<PerMeter> efficiency,
		const std::string& textureFile,
		const std::string& innerfillTextureFile,
		const std::string& coolantfillTextureFile,
		const float_s textureScale
	) noexcept;

	void dumpCustomProperties(Def::DataDumper& dump) const override final;
};
