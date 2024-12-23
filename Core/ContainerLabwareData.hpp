#pragma once

#include "BaseContainerLabwareData.hpp"
#include "ShapeFillTexture.hpp"
#include "ShapeFill.hpp"
#include "PathUtils.hpp"
#include "Amount.hpp"
#include "Ref.hpp"

#include <type_traits>
#include <array>

template<uint8_t C>
class ContainerLabwareData : public BaseContainerLabwareData
{
private:
	const std::array<Amount<Unit::LITER>, C> volumes;
	std::array<ShapeFillTexture, C> fillTextures;

	/// <summary>
	/// Statically builds a ShapeFill array for every container.
	/// ...went a little overboard with this...
	/// </summary>
	template<uint8_t... I>
	constexpr std::array<ShapeFill, C> generateShapeFills(std::index_sequence<I...>) const;

protected:
	template<typename = std::enable_if_t<C >= 1>>
	ContainerLabwareData(
		const LabwareId id,
		const std::string& name,
		std::vector<LabwarePort>&& ports,
		const std::string& textureFile,
		const float_s textureScale,
		std::array<Amount<Unit::LITER>, C>&& volumes,
		const std::array<Ref<const std::string>, C>& fillTextureFiles,
		const LabwareType type
	) noexcept;

	template<typename = std::enable_if_t<C == 1>>
	ContainerLabwareData(
		const LabwareId id,
		const std::string& name,
		std::vector<LabwarePort>&& ports,
		const std::string& textureFile,
		const float_s textureScale,
		const Amount<Unit::LITER> volume,
		const LabwareType type
	) noexcept;

public:
	constexpr Amount<Unit::LITER> getVolume() const override final;
	constexpr const ShapeFillTexture& getFillTexture() const override final;

	template<uint8_t I, typename = std::enable_if_t<I < C>>
	constexpr Amount<Unit::LITER> getVolume() const;
	template<uint8_t I, typename = std::enable_if_t<I < C>>
	constexpr const ShapeFillTexture& getFillTexture() const;

	constexpr const std::array<Amount<Unit::LITER>, C>& getVolumes() const;
	constexpr const std::array<ShapeFillTexture, C>& getFillTextures() const;
	constexpr const std::array<ShapeFill, C> generateShapeFills() const;

	void dumpTextures(const std::string& path) const override final;
};


template<uint8_t C>
template<typename>
ContainerLabwareData<C>::ContainerLabwareData(
	const LabwareId id,
	const std::string& name,
	std::vector<LabwarePort>&& ports,
	const std::string& textureFile,
	const float_s textureScale,
	std::array<Amount<Unit::LITER>, C>&& volumes,
	const std::array<Ref<const std::string>, C>& fillTextureFiles,
	const LabwareType type
) noexcept :
	BaseContainerLabwareData(id, name, std::move(ports), textureFile, textureScale, type),
	volumes(std::move(volumes))
{
	for (uint8_t i = 0; i < C; ++i)
		fillTextures[i] = ShapeFillTexture(*fillTextureFiles[i], 0, hasMultiLayerStorage(type));
}

template<uint8_t C>
template<typename>
ContainerLabwareData<C>::ContainerLabwareData(
	const LabwareId id,
	const std::string& name,
	std::vector<LabwarePort>&& ports,
	const std::string& textureFile,
	const float_s textureScale,
	const Amount<Unit::LITER> volume,
	const LabwareType type
) noexcept :
	BaseContainerLabwareData(id, name, std::move(ports), textureFile, textureScale, type),
	volumes({ volume })
{
	fillTextures.front() = ShapeFillTexture(texture, 0, hasMultiLayerStorage(type));
}

template<uint8_t C>
template<uint8_t I, typename>
constexpr Amount<Unit::LITER> ContainerLabwareData<C>::getVolume() const
{
	return volumes[I];
}

template<uint8_t C>
template<uint8_t I, typename>
constexpr const ShapeFillTexture& ContainerLabwareData<C>::getFillTexture() const
{
	return fillTextures[I];
}

template<uint8_t C>
constexpr Amount<Unit::LITER>  ContainerLabwareData<C>::getVolume() const
{
	return volumes.front();
}

template<uint8_t C>
constexpr const ShapeFillTexture& ContainerLabwareData<C>::getFillTexture() const
{
	return fillTextures.front();
}

template<uint8_t C>
constexpr const std::array<Amount<Unit::LITER>, C>& ContainerLabwareData<C>::getVolumes() const
{
	return volumes;
}

template<uint8_t C>
constexpr const std::array<ShapeFillTexture, C>& ContainerLabwareData<C>::getFillTextures() const
{
	return fillTextures;
}

template<uint8_t C>
template<uint8_t... I>
constexpr std::array<ShapeFill, C> ContainerLabwareData<C>::generateShapeFills(std::index_sequence<I...>) const
{
	return { { ShapeFill(std::get<I>(fillTextures), textureScale)...} };
}

template<uint8_t C>
constexpr const std::array<ShapeFill, C> ContainerLabwareData<C>::generateShapeFills() const
{
	return generateShapeFills(std::make_index_sequence<C>());
}

template<uint8_t C>
void ContainerLabwareData<C>::dumpTextures(const std::string& path) const
{
	DrawableLabwareData::dumpTextures(path);
	for (uint8_t i = 0; i < C; ++i)
	{
		fillTextures[i].getTexture().copyToImage().saveToFile(
			Utils::combinePaths(path, "fill" + std::to_string(i) + '_' + textureFile));
	}
}
