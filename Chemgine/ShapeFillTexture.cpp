#include "ShapeFillTexture.hpp"
#include "Collision.hpp"

ShapeFillTexture::ShapeFillTexture(
	const sf::Texture& source,
	const uint8_t alphaThreshold,
	const bool enableVolumetricScaling
) noexcept :
	texture(new sf::Texture())
{
	const auto& mask = Collision::getTextureMask(source);
	const auto size = source.getSize();

	sf::Image img;
	img.create(size.x, size.y, sf::Color::Transparent);

	// build pixel-wise volume for every texture line
	uint32_t totalSetPixels = 0;
	std::vector<std::pair<float, float>> pixels(size.y + 1, { 0.0f, 0.0f });

	for (uint32_t i = 0; i < size.y; ++i)
	{
		uint32_t l = 0;
		while (++l < size.x && mask[i * size.x + l] <= alphaThreshold);

		uint32_t r = size.x;
		while (--r > l && mask[i * size.x + r] <= alphaThreshold);

		for (uint32_t j = l; j < r; ++j)
		{
			img.setPixel(j, i, sf::Color::White);
			++pixels[size.y - i].first;
			++totalSetPixels;
		}
	}

	for (uint32_t j = 0; j < size.x; ++j)
	{
		uint32_t t = 0;
		while (++t < size.y && mask[t * size.x + j] <= alphaThreshold)
		{
			if (img.getPixel(j, t) == sf::Color::Transparent)
				continue;

			img.setPixel(j, t, sf::Color::Transparent);
			--pixels[size.y - t].first;
		}

		uint32_t b = size.y;
		while (--b > t && mask[b * size.x + j] <= alphaThreshold)
		{
			if (img.getPixel(j, b) == sf::Color::Transparent)
				continue;

			img.setPixel(j, b, sf::Color::Transparent);
			--pixels[size.y - b].first;
		}
	}

	if (enableVolumetricScaling)
	{
		for (size_t i = 1; i < size.y; ++i)
		{
			pixels[i].first = pixels[i].first / static_cast<float>(totalSetPixels) + pixels[i - 1].first;
			pixels[i].second = i / static_cast<float>(size.y);
		}
		pixels.back().first = 1.0f;
		pixels.back().second = 1.0f;

		volume = Spline<float>(std::move(pixels), 0.002f);
	}

	texture->loadFromImage(img);
}

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