#include "labware/data/HeatsourceData.hpp"

HeatsourceData::HeatsourceData(
    const LabwareId            id,
    const std::string&         name,
    std::vector<LabwarePort>&& ports,
    const Amount<Unit::WATT>   maxPowerOutput,
    const std::string&         textureFile,
    const float_s              textureScale) noexcept :
    DrawableLabwareData(id, name, std::move(ports), textureFile, textureScale, LabwareType::HEATSOURCE),
    maxPowerOutput(maxPowerOutput)
{}

void HeatsourceData::dumpCustomProperties(def::DataDumper& dump) const
{
    dump.propertyWithSep(def::Labware::Power, maxPowerOutput)
        .propertyWithSep(def::Labware::Texture, "~/" + textureFile)
        .property(def::Labware::TextureScale, textureScale);
}
