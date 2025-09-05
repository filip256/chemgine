#pragma once

#include "labware/data/ContainerLabwareData.hpp"

class CondenserData : public ContainerLabwareData<2>
{
public:
    const Amount<Unit::METER>     length;
    const Amount<Unit::PER_METER> efficiency;

    CondenserData(
        const LabwareId               id,
        const std::string&            name,
        std::vector<LabwarePort>&&    ports,
        const Amount<Unit::LITER>     innerVolume,
        const Amount<Unit::METER>     length,
        const Amount<Unit::PER_METER> efficiency,
        const std::string&            textureFile,
        const std::string&            innerfillTextureFile,
        const std::string&            coolantfillTextureFile,
        const float_s                 textureScale) noexcept;

    void dumpCustomProperties(def::DataDumper& dump) const override final;
};
