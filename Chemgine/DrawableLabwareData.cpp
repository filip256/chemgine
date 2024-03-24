#include "DrawableLabwareData.hpp"
#include "Collision.hpp"
#include "ShapeFill.hpp"

DrawableLabwareData::DrawableLabwareData(
	const LabwareId id,
	const std::string& name,
	std::vector<LabwarePort>&& ports,
	const std::string& textureFile,
	const float textureScale,
	const LabwareType type
) noexcept :
	BaseLabwareData(id, name, std::move(ports), type),
	textureScale(textureScale)
{
	Collision::createTextureAndBitmask(texture, textureFile);
}
