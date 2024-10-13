#include "DrawableLabwareData.hpp"
#include "Collision.hpp"
#include "ShapeFill.hpp"
#include "PathUtils.hpp"

DrawableLabwareData::DrawableLabwareData(
	const LabwareId id,
	const std::string& name,
	std::vector<LabwarePort>&& ports,
	const std::string& textureFile,
	const float_n textureScale,
	const LabwareType type
) noexcept :
	BaseLabwareData(id, name, std::move(ports), type),
	textureScale(textureScale),
	textureFile(Utils::extractFileName(textureFile))
{
	Collision::createTextureAndBitmask(texture, textureFile);
}

void DrawableLabwareData::dumpTextures(const std::string& path) const
{
	texture.copyToImage().saveToFile(Utils::combinePaths(path, textureFile));
}
