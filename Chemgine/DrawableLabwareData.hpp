#pragma once

#include "BaseLabwareData.hpp"
#include "SFML/Graphics/Texture.hpp"

class DrawableLabwareData : public BaseLabwareData
{
protected:
	DrawableLabwareData(
		const LabwareId id,
		const std::string& name,
		std::vector<LabwarePortData>&& ports,
		std::vector<LabwareContactData>&& contacts,
		const std::string& textureFile,
		const float textureScale,
		const LabwareType type
	) noexcept;

public:
	const float textureScale;
	sf::Texture texture;
};
