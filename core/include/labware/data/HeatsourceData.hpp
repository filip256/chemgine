#pragma once

#include "data/values/Amount.hpp"
#include "labware/data/DrawableLabwareData.hpp"

class HeatsourceData : public DrawableLabwareData
{
public:
    const Amount<Unit::WATT> maxPowerOutput;

    HeatsourceData(
        const LabwareId            id,
        const std::string&         name,
        std::vector<LabwarePort>&& ports,
        const Amount<Unit::WATT>   maxPowerOutput,
        const std::string&         textureFile,
        const float_s              textureScale) noexcept;

    void dumpCustomProperties(def::DataDumper& dump) const override final;
};
