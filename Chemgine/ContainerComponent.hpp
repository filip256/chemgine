#pragma once

#include "BaseContainerComponent.hpp"
#include "FlaskData.hpp"
#include "Reactor.hpp"

#include <type_traits>

template<typename C>
class ContainerComponent : public BaseContainerComponent
{
	static_assert(std::is_base_of_v<Mixture, C> || std::is_same_v<Mixture, C>,
		"C must be a mixture type");

protected:
	C container;

	ContainerComponent(
		const LabwareId id,
		C&& container
	) noexcept;

	inline ContainerComponent(
		const LabwareId id,
		Atmosphere& atmosphere
	) noexcept = delete;

public:
	const ContainerLabwareData& getData() const override;

	const C& getContent() const override final;
	C& getContent() override final;

	void add(const Molecule& molecule, const Amount<Unit::MOLE> amount) override final;
	void add(const Amount<Unit::JOULE> energy) override final;

	Ref<BaseContainer> getOverflowTarget() const override final;
	void setOverflowTarget(const Ref<BaseContainer> target) override final;
	void setOverflowTarget(BaseContainerComponent& target) override final;

	void tick(const Amount<Unit::SECOND> timespan) override final;
};


template<typename C>
ContainerComponent<C>::ContainerComponent(
	const LabwareId id,
	C&& container
) noexcept :
	BaseContainerComponent(id),
	container(std::move(container))
{}

template<>
inline ContainerComponent<Atmosphere>::ContainerComponent(
	const LabwareId id,
	Atmosphere& atmosphere
) noexcept :
	BaseContainerComponent(id),
	container(atmosphere.createSubatmosphere(getData().volume))
{}

template<>
inline ContainerComponent<Reactor>::ContainerComponent(
	const LabwareId id,
	Atmosphere& atmosphere
) noexcept :
	BaseContainerComponent(id),
	container(Reactor(atmosphere, getData().volume, atmosphere))
{}

template<typename C>
const ContainerLabwareData& ContainerComponent<C>::getData() const
{
	return static_cast<const ContainerLabwareData&>(data);
}

template<typename C>
const C& ContainerComponent<C>::getContent() const 
{
	return container;
}

template<typename C>
C& ContainerComponent<C>::getContent() 
{
	return container;
}

template<typename C>
void ContainerComponent<C>::add(const Molecule& molecule, const Amount<Unit::MOLE> amount) 
{
	container.add(molecule, amount);
}

template<typename C>
void ContainerComponent<C>::add(const Amount<Unit::JOULE> energy)
{
	container.add(energy);
}

template<typename C>
Ref<BaseContainer> ContainerComponent<C>::getOverflowTarget() const 
{
	return container.getOverflowTarget();
}

template<typename C>
void ContainerComponent<C>::setOverflowTarget(const Ref<BaseContainer> target) 
{
	container.setOverflowTarget(target);
}

template<typename C>
void ContainerComponent<C>::setOverflowTarget(BaseContainerComponent& target)
{
	BaseContainerComponent::setOverflowTarget(target);
}

template<typename C>
void ContainerComponent<C>::tick(const Amount<Unit::SECOND> timespan)
{
	container.tick(timespan);
}
