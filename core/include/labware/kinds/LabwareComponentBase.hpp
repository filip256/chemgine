#pragma once

#include "data/Accessor.hpp"
#include "data/DataStoreAccessor.hpp"
#include "labware/DrawablePort.hpp"
#include "labware/data/BaseLabwareData.hpp"
#include "mixtures/kinds/Mixture.hpp"
#include "molecules/kinds/Molecule.hpp"

#include <SFML/Graphics.hpp>

class LabwareComponentBase : public Accessor<>,
                             public sf::Drawable
{
public:
    const LabwareId id;

protected:
    const BaseLabwareData& data;

public:
    LabwareComponentBase(const LabwareId id, const LabwareType type) noexcept;

    virtual ~LabwareComponentBase() = default;

    bool         isFlask() const;
    bool         isAdaptor() const;
    bool         isCondenser() const;
    bool         isHeatsource() const;
    virtual bool isContainer() const;

    virtual const BaseLabwareData& getData() const                               = 0;
    virtual const sf::Sprite&      getSprite() const                             = 0;
    virtual sf::Sprite&            getSprite()                                   = 0;
    virtual sf::Vector2f           getPosition() const                           = 0;
    virtual sf::Vector2f           getAdjustedPosition() const                   = 0;
    virtual void                   setPosition(const sf::Vector2f& position)     = 0;
    virtual void                   move(const sf::Vector2f& offset)              = 0;
    virtual Amount<Unit::DEGREE>   getRotation() const                           = 0;
    virtual void                   setRotation(const Amount<Unit::DEGREE> angle) = 0;
    virtual sf::Vector2f           getOrigin() const                             = 0;
    virtual sf::FloatRect          getBounds() const                             = 0;

    virtual const DrawablePort&              getPort(const uint8_t idx) const = 0;
    virtual const std::vector<DrawablePort>& getPorts() const                 = 0;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
    virtual bool contains(const sf::Vector2f& point) const                     = 0;
    virtual bool intersects(const LabwareComponentBase& other) const           = 0;

    virtual bool tryConnect(LabwareComponentBase& other);
    virtual void disconnect(const Ref<ContainerBase> dump, const LabwareComponentBase& other);

    virtual void tick(const Amount<Unit::SECOND> timespan);

    template <typename L, typename = std::enable_if<std::is_base_of_v<LabwareComponentBase, L>>>
    L& as();
    template <typename L, typename = std::enable_if<std::is_base_of_v<LabwareComponentBase, L>>>
    Ref<L> cast();
};

template <typename L, typename>
L& LabwareComponentBase::as()
{
    return Ref(*this).template as<L>();
}

template <typename L, typename>
Ref<L> LabwareComponentBase::cast()
{
    return Ref(*this).template cast<L>();
}
