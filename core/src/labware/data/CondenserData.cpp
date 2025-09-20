#include "labware/data/CondenserData.hpp"

CondenserData::CondenserData(
    const LabwareId               id,
    const std::string&            name,
    std::vector<LabwarePort>&&    ports,
    const Amount<Unit::LITER>     innerVolume,
    const Amount<Unit::METER>     length,
    const Amount<Unit::PER_METER> efficiency,
    const std::string&            textureFile,
    const std::string&            innerfillTextureFile,
    const std::string&            coolantfillTextureFile,
    const float_s                 textureScale) noexcept :
    ContainerLabwareData<2>(
        id,
        name,
        std::move(ports),
        textureFile,
        textureScale,
        {innerVolume, innerVolume},
        {Ref(innerfillTextureFile), Ref(coolantfillTextureFile)},
        LabwareType::CONDENSER),
    length(length),
    efficiency(efficiency)
{}

void CondenserData::dumpCustomProperties(def::DataDumper& dump) const
{
    dump.property(def::Labware::Volume, getVolume<0>())
        .property(def::Labware::Length, length)
        .property(def::Labware::Efficiency, efficiency)
        .property(def::Labware::Texture, "~/" + textureFile)
        .property(def::Labware::InnerMask, "~/fill0_" + textureFile)
        .property(def::Labware::CoolantMask, "~/fill1_" + textureFile)
        .property(def::Labware::TextureScale, textureScale);
}
