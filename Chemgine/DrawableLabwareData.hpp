#pragma once

#include "BaseLabwareData.hpp"
#include "ShapeFillTexture.hpp"

class DrawableLabwareData : public BaseLabwareData
{
protected:
	DrawableLabwareData(
		const LabwareId id,
		const std::string& name,
		std::vector<LabwarePort>&& ports,
		const std::string& textureFile,
		const LabwareType type
	) noexcept;

public:
	sf::Texture texture;
	ShapeFillTexture fillTexture;
};
