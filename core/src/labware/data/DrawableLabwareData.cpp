#include "labware/data/DrawableLabwareData.hpp"

#include "graphics/Collision.hpp"
#include "graphics/ShapeFill.hpp"
#include "utils/Path.hpp"

DrawableLabwareData::DrawableLabwareData(
    const LabwareId            id,
    const std::string&         name,
    std::vector<LabwarePort>&& ports,
    const std::string&         textureFile,
    const float_s              textureScale,
    const LabwareType          type) noexcept :
    BaseLabwareData(id, name, std::move(ports), type),
    textureScale(textureScale),
    textureFile(utils::extractFileNameWithExtension(textureFile))
{
    Collision::createTextureAndBitmask(texture, textureFile);
}

void DrawableLabwareData::dumpTextures(const std::string& path) const
{
    const auto txPath = utils::combinePaths(path, textureFile);
    if (not texture.copyToImage().saveToFile(txPath))
        Log(this).fatal("Failed to dump texture to file: {}.", txPath);
}
