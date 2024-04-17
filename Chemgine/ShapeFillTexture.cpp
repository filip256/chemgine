#include "ShapeFillTexture.hpp"
#include "Collision.hpp"

ShapeFillTexture::ShapeFillTexture(
	const Collision::SizedTextureMask& textureMask,
	const uint8_t alphaThreshold,
	const bool enableVolumetricScaling
) noexcept :
	texture(std::make_unique<sf::Texture>())
{
	const auto& mask = textureMask.first;
	const auto maskSize = textureMask.second;

	sf::Image img;
	img.create(maskSize.x, maskSize.y, sf::Color::Transparent);

	// build pixel-wise volume for every texture line
	uint32_t totalSetPixels = 0;
	std::vector<std::pair<float, float>> pixels(maskSize.y + 1, { 0.0f, 0.0f });

	for (uint32_t i = 0; i < maskSize.y; ++i)
	{
		uint32_t l = 0;
		while (++l < maskSize.x && mask[i * maskSize.x + l] <= alphaThreshold);

		uint32_t r = maskSize.x;
		while (--r > l && mask[i * maskSize.x + r] <= alphaThreshold);

		for (uint32_t j = l; j < r; ++j)
		{
			img.setPixel(j, i, sf::Color::White);
			++pixels[maskSize.y - i].first;
			++totalSetPixels;
		}
	}

	for (uint32_t j = 0; j < maskSize.x; ++j)
	{
		uint32_t t = 0;
		while (++t < maskSize.y && mask[t * maskSize.x + j] <= alphaThreshold)
		{
			if (img.getPixel(j, t) == sf::Color::Transparent)
				continue;

			img.setPixel(j, t, sf::Color::Transparent);
			--pixels[maskSize.y - t].first;
		}

		uint32_t b = maskSize.y;
		while (--b > t && mask[b * maskSize.x + j] <= alphaThreshold)
		{
			if (img.getPixel(j, b) == sf::Color::Transparent)
				continue;

			img.setPixel(j, b, sf::Color::Transparent);
			--pixels[maskSize.y - b].first;
		}
	}

	if (enableVolumetricScaling)
	{
		for (size_t i = 1; i < maskSize.y; ++i)
		{
			pixels[i].first = pixels[i].first / static_cast<float>(totalSetPixels) + pixels[i - 1].first;
			pixels[i].second = i / static_cast<float>(maskSize.y);
		}
		pixels.back().first = 1.0f;
		pixels.back().second = 1.0f;

		volume = Spline<float>(std::move(pixels), 0.002f);
	}

	texture->loadFromImage(img);
}

ShapeFillTexture::ShapeFillTexture(
	const sf::Texture& source,
	const uint8_t alphaThreshold,
	const bool enableVolumetricScaling
) noexcept :
	ShapeFillTexture(
		std::make_pair(Collision::getTextureMask(source), source.getSize()),
		alphaThreshold, enableVolumetricScaling)
{}

ShapeFillTexture::ShapeFillTexture(
	const std::string& sourceFile,
	const uint8_t alphaThreshold,
	const bool enableVolumetricScaling
) noexcept :
	ShapeFillTexture(
		Collision::createBitmask(sourceFile),
		alphaThreshold, enableVolumetricScaling)
{}


ShapeFillTexture& ShapeFillTexture::operator=(ShapeFillTexture&& other) noexcept
{
	if (this == &other)
		return *this;

	this->texture.reset(other.texture.release());
	this->volume = std::move(other.volume);

	return *this;
}

const sf::Texture& ShapeFillTexture::getTexture() const
{
	return *texture;
}

bool ShapeFillTexture::hasVolumetricScaling() const
{
	return volume.size();
}

float ShapeFillTexture::getRelativeHeightAt(const float fillRatio) const
{
	return volume.getLinearValueAt(fillRatio);
}