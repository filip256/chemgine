#pragma once

#include "BaseContainerComponent.hpp"
#include "ContainerLabwareData.hpp"

#include <type_traits>
#include <tuple>

template<typename... Args>
class ContainerComponent : public BaseContainerComponent
{
protected:
	static constexpr uint8_t ContainerCount = sizeof...(Args);
	using DataObjectT = typename ContainerLabwareData<ContainerCount>;

private:
	std::array<ShapeFill, ContainerCount> fills;
	std::tuple<Args...> containers;

protected:
	template<typename... CArgs, typename =
		std::enable_if_t<std::conjunction_v<std::is_base_of<Mixture, Args>...>>>
	ContainerComponent(
		const LabwareId id,
		const LabwareType type,
		CArgs&&... containers
	) noexcept;

	inline ContainerComponent(
		const LabwareId id,
		const LabwareType type,
		Atmosphere& atmosphere
	) noexcept = delete;

	Ref<BaseContainer> getOverflowTarget() const override final;
	void setOverflowTarget(const Ref<BaseContainer> target) override final;
	void setOverflowTarget(BaseContainerComponent& target) override final;

	template<uint8_t I>
	constexpr const auto& getContent() const;
	template<uint8_t I>
	constexpr auto& getContent();

	template<uint8_t I>
	constexpr const auto& getFill() const;
	template<uint8_t I>
	constexpr auto& getFill();

public:
	const DataObjectT& getData() const override;

	void setPosition(const sf::Vector2f& position) override final;
	void move(const sf::Vector2f& offset) override final;
	void setRotation(const Amount<Unit::DEGREE> angle) override final;

	constexpr const Mixture& getContent() const override final;
	constexpr Mixture& getContent() override final;

	void add(const Molecule& molecule, const Amount<Unit::MOLE> amount) override final;
	void add(const Amount<Unit::JOULE> energy) override final;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void tick(const Amount<Unit::SECOND> timespan) override;
};


template<typename... Args>
template<typename... CArgs, typename>
ContainerComponent<Args...>::ContainerComponent(
	const LabwareId id,
	const LabwareType type,
	CArgs&&... containers
) noexcept :
	BaseContainerComponent(id, type),
	containers(std::forward<CArgs>(containers)...),
	fills(getData().generateShapeFills())
{}

template<>
inline ContainerComponent<Atmosphere>::ContainerComponent(
	const LabwareId id,
	const LabwareType type,
	Atmosphere& atmosphere
) noexcept :
	BaseContainerComponent(id, type),
	containers(atmosphere.createSubatmosphere(getData().getVolume())),
	fills(getData().generateShapeFills())
{}

template<>
inline ContainerComponent<Reactor>::ContainerComponent(
	const LabwareId id,
	const LabwareType type,
	Atmosphere& atmosphere
) noexcept :
	BaseContainerComponent(id, type),
	containers(Reactor(atmosphere, getData().getVolume(), atmosphere)),
	fills(getData().generateShapeFills())
{}

template<>
inline ContainerComponent<Atmosphere, Reactor>::ContainerComponent(
	const LabwareId id,
	const LabwareType type,
	Atmosphere& atmosphere
) noexcept :
	BaseContainerComponent(id, type),
	containers(std::make_tuple(
		atmosphere.createSubatmosphere(getData().getVolume()),
		Reactor(atmosphere, getData().getVolume(), atmosphere))),
	fills(getData().generateShapeFills())
{}

template<typename... Args>
const typename ContainerComponent<Args...>::DataObjectT& ContainerComponent<Args...>::getData() const
{
	return static_cast<const DataObjectT&>(data);
}

template<typename... Args>
void ContainerComponent<Args...>::setPosition(const sf::Vector2f& position)
{
	std::for_each(fills.begin(), fills.end(),
		[&position](auto& f) { f.setPosition(position); });

	DrawableComponent::setPosition(position);
}

template<typename... Args>
void ContainerComponent<Args...>::move(const sf::Vector2f& offset)
{
	std::for_each(fills.begin(), fills.end(),
		[&offset](auto& f) { f.move(offset); });

	DrawableComponent::move(offset);
}

template<typename... Args>
void ContainerComponent<Args...>::setRotation(const Amount<Unit::DEGREE> angle)
{
	std::for_each(fills.begin(), fills.end(),
		[angle](auto& f) { f.setRotation(angle); });

	DrawableComponent::setRotation(angle);
}

template<typename... Args>
constexpr const Mixture& ContainerComponent<Args...>::getContent() const
{
	return std::get<0>(containers);
}

template<typename... Args>
constexpr Mixture& ContainerComponent<Args...>::getContent()
{
	return std::get<0>(containers);
}

template<typename... Args>
template<uint8_t I>
constexpr const auto& ContainerComponent<Args...>::getContent() const
{
	return std::get<I>(containers);
}

template<typename... Args>
template<uint8_t I>
constexpr auto& ContainerComponent<Args...>::getContent()
{
	return std::get<I>(containers);
}

template<typename... Args>
template<uint8_t I>
constexpr const auto& ContainerComponent<Args...>::getFill() const
{
	return std::get<I>(fills);
}

template<typename... Args>
template<uint8_t I>
constexpr auto& ContainerComponent<Args...>::getFill()
{
	return std::get<I>(fills);
}

template<typename... Args>
void ContainerComponent<Args...>::add(const Molecule& molecule, const Amount<Unit::MOLE> amount)
{
	getContent<0>().add(molecule, amount);
}

template<typename... Args>
void ContainerComponent<Args...>::add(const Amount<Unit::JOULE> energy)
{
	getContent<0>().add(energy);
}

template<typename... Args>
Ref<BaseContainer> ContainerComponent<Args...>::getOverflowTarget() const
{
	return getContent<0>().getOverflowTarget();
}

template<typename... Args>
void ContainerComponent<Args...>::setOverflowTarget(const Ref<BaseContainer> target)
{
	getContent<0>().setOverflowTarget(target);
}

template<typename... Args>
void ContainerComponent<Args...>::setOverflowTarget(BaseContainerComponent& target)
{
	BaseContainerComponent::setOverflowTarget(target);
}

template<typename... Args>
void ContainerComponent<Args...>::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	uint8_t idx = 0;
	std::apply([this, &target, &states, &idx](const auto&... c) 
	{
		((BaseContainerComponent::draw(c, fills[idx++], target, states)), ...);
	}, containers);

	DrawableComponent::draw(target, states);
}

template<typename... Args>
void ContainerComponent<Args...>::tick(const Amount<Unit::SECOND> timespan)
{
	std::apply([timespan](auto&... c) {(c.tick(timespan), ...); }, containers);
}
