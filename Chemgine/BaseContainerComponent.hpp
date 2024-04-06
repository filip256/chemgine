#pragma once

#include "DrawableComponent.hpp"
#include "ContainerLabwareData.hpp"

class BaseContainerComponent : public DrawableComponent
{
protected:
	ShapeFill fill;

	BaseContainerComponent(const LabwareId id) noexcept;

public:
	const ContainerLabwareData& getData() const override;

	bool isContainer() const override final;

	void setPosition(const sf::Vector2f& position) override final;
	void move(const sf::Vector2f& offset) override final;
	void setRotation(const float angle) override final;

	virtual const Mixture& getContent() const = 0;
	virtual Mixture& getContent() = 0;

	virtual void add(const Molecule& molecule, const Amount<Unit::MOLE> amount) = 0;
	virtual void add(const Amount<Unit::JOULE> energy) = 0;

	virtual Ref<BaseContainer> getOverflowTarget() const = 0;
	virtual void setOverflowTarget(const Ref<BaseContainer> target) = 0;
	virtual void setOverflowTarget(BaseContainerComponent& target);
};
