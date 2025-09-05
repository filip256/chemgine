#include "labware/data/FlaskData.hpp"

#include <iostream>

FlaskData::FlaskData(
    const LabwareId            id,
    const std::string&         name,
    std::vector<LabwarePort>&& ports,
    const Amount<Unit::LITER>  volume,
    const std::string&         textureFile,
    const float_s              textureScale) noexcept :
    ContainerLabwareData(id, name, std::move(ports), textureFile, textureScale, volume, LabwareType::FLASK)
{}

void FlaskData::dumpCustomProperties(def::DataDumper& dump) const
{
    dump.property(def::Labware::Volume, getVolume())
        .property(def::Labware::Texture, "~/" + textureFile)
        .property(def::Labware::TextureScale, textureScale);
}
