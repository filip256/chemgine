#pragma once

#include "BaseLabwareData.hpp"
#include "SFML/Graphics/Texture.hpp"

class DrawableLabwareData : public BaseLabwareData
{
protected:
	DrawableLabwareData(
		const LabwareId id,
		const std::string& name,
		std::vector<LabwarePort>&& ports,
		const std::string& textureFile,
		const float textureScale,
		const LabwareType type
	) noexcept;

public:
	const float textureScale;
	const std::string textureFile;
	sf::Texture texture;

	void dumpTextures(const std::string& path) const override;
};
