#pragma once

#include "BaseLabwareData.hpp"
#include "SFML/Graphics.hpp"

class DrawableLabwareData : public BaseLabwareData
{
protected:
	DrawableLabwareData(
		const LabwareIdType id,
		const std::string& name,
		std::vector<LabwarePort>&& ports,
		const std::string& textureFile,
		const LabwareType type
	) noexcept;

public:
	sf::Texture texture;
};