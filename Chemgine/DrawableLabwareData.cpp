#include "DrawableLabwareData.hpp"
#include "Collision.hpp"

DrawableLabwareData::DrawableLabwareData(
	const LabwareIdType id,
	const std::string& name,
	const std::string& textureFile,
	const LabwareType type
) noexcept :
	BaseLabwareData(id, name, type)
{
	Collision::createTextureAndBitmask(texture, textureFile);
}