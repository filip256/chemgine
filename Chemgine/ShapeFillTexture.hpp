#pragma once

#include "SFML/Graphics.hpp"
#include "Spline.hpp"

#include <memory>

class ShapeFillTexture
{
private:
	std::unique_ptr<sf::Texture> texture;
	Spline<float> volume;

public:
	ShapeFillTexture(
		const sf::Texture& source,
		const uint8_t alphaThreshold,
		const bool enableVolumetricScaling
	) noexcept;
	ShapeFillTexture() = default;
	ShapeFillTexture(const ShapeFillTexture&) = delete;
	ShapeFillTexture(ShapeFillTexture&& other) = default;

	ShapeFillTexture& operator=(ShapeFillTexture&& other) noexcept;

	const sf::Texture& getTexture() const;

	bool hasVolumetricScaling() const;
	float getRelativeHeightAt(const float fillRatio) const;
};
