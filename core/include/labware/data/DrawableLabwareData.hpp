#pragma once

#include "labware/data/BaseLabwareData.hpp"

#include <SFML/Graphics/Texture.hpp>

class DrawableLabwareData : public BaseLabwareData
{
protected:
	DrawableLabwareData(
		const LabwareId id,
		const std::string& name,
		std::vector<LabwarePort>&& ports,
		const std::string& textureFile,
		const float_s textureScale,
		const LabwareType type
	) noexcept;

public:
	const float_s textureScale;
	const std::string textureFile;
	sf::Texture texture;

	void dumpTextures(const std::string& path) const override;
};
