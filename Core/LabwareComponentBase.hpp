#pragma once

#include "BaseLabwareData.hpp"
#include "DataStoreAccessor.hpp"
#include "DrawablePort.hpp"
#include "Molecule.hpp"
#include "Mixture.hpp"
#include "Accessor.hpp"
#include "SFML/Graphics.hpp"

class LabwareComponentBase : public Accessor<>, public sf::Drawable
{
protected:
	const BaseLabwareData& data;

	LabwareComponentBase(
		const LabwareId id,
		const LabwareType type
	) noexcept;

public:
	const LabwareId id;

	virtual ~LabwareComponentBase() = default;

	bool isFlask() const;
	bool isAdaptor() const;
	bool isCondenser() const;
	bool isHeatsource() const;
	virtual bool isContainer() const;

	virtual const BaseLabwareData& getData() const = 0;
	virtual const sf::Sprite& getSprite() const = 0;
	virtual sf::Sprite& getSprite() = 0;
	virtual const sf::Vector2f& getPosition() const = 0;
	virtual const sf::Vector2f getAdjustedPosition() const = 0;
	virtual void setPosition(const sf::Vector2f& position) = 0;
	virtual void move(const sf::Vector2f& offset) = 0;
	virtual Amount<Unit::DEGREE> getRotation() const = 0;
	virtual void setRotation(const Amount<Unit::DEGREE> angle) = 0;
	virtual const sf::Vector2f& getOrigin() const = 0;
	virtual sf::FloatRect getBounds() const = 0;

	virtual const DrawablePort& getPort(const uint8_t idx) const = 0;
	virtual const std::vector<DrawablePort>& getPorts() const = 0;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
	virtual bool contains(const sf::Vector2f& point) const = 0;
	virtual bool intersects(const LabwareComponentBase& other) const = 0;

	virtual bool tryConnect(LabwareComponentBase& other);
	virtual void disconnect(const Ref<ContainerBase> dump, const LabwareComponentBase& other);

	virtual void tick(const Amount<Unit::SECOND> timespan);

	template<typename L, typename = std::enable_if<std::is_base_of_v<LabwareComponentBase, L>>>
	L& as();
	template<typename L, typename = std::enable_if<std::is_base_of_v<LabwareComponentBase, L>>>
	Ref<L> cast();
};

template<typename L, typename>
L& LabwareComponentBase::as()
{
	return Ref(*this).as<L>();
}

template<typename L, typename>
Ref<L> LabwareComponentBase::cast()
{
	return Ref(*this).cast<L>();
}
