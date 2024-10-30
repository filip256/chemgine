#pragma once

#include "ContainerLabwareData.hpp"

class AdaptorData : public ContainerLabwareData<1>
{
public:
	AdaptorData(
		const LabwareId id,
		const std::string& name,
		std::vector<LabwarePort>&& ports,
		const Amount<Unit::LITER> volume,
		const std::string& textureFile,
		const float_n textureScale
	) noexcept;

	void dumpCustomProperties(Def::DataDumper& dump) const override final;
};
