#pragma once

#include "labware/data/BaseContainerLabwareData.hpp"
#include "labware/kinds/DrawableComponent.hpp"
#include "mixtures/kinds/Reactor.hpp"
#include "utils/SFML.hpp"

class BaseContainerComponent : public DrawableComponent
{
protected:
    BaseContainerComponent(const LabwareId id, const LabwareType type) noexcept;

    virtual Ref<ContainerBase> getOverflowTarget() const                          = 0;
    virtual void               setOverflowTarget(const Ref<ContainerBase> target) = 0;
    virtual void               setOverflowTarget(BaseContainerComponent& target);

    template <typename C, typename = std::enable_if_t<std::is_base_of_v<Mixture, C>>>
    static inline void draw(
        const C& container,
        const ShapeFill& fill,
        sf::RenderTarget& target,
        const sf::RenderStates& states) = delete;

public:
    const BaseContainerLabwareData& getData() const override;

    bool isContainer() const override final;

    virtual const Mixture& getContent() const = 0;
    virtual Mixture&       getContent()       = 0;

    virtual void add(const Molecule& molecule, const Amount<Unit::MOLE> amount) = 0;
    virtual void addEnergy(const Amount<Unit::JOULE> energy)                    = 0;
};

template <>
inline void BaseContainerComponent::draw<Atmosphere>(
    const Atmosphere&       container,
    const ShapeFill&        fill,
    sf::RenderTarget&       target,
    const sf::RenderStates& states)
{
    fill.setColor(utils::colorCast(container.getLayerColor()));
    target.draw(fill, states);
}

template <>
inline void BaseContainerComponent::draw<Reactor>(
    const Reactor&          container,
    const ShapeFill&        fill,
    sf::RenderTarget&       target,
    const sf::RenderStates& states)
{
    float_s lastSection = 0.0f;
    for (auto l = container.getLayersUpBegin(); l != container.getLayersUpEnd(); ++l) {
        const auto layerSection = (l->second.getVolume() / container.getMaxVolume()).asStd();
        fill.setDrawSection(
            lastSection, lastSection + layerSection, utils::colorCast(l->second.getColor()));
        lastSection += layerSection;

        target.draw(fill, states);
    }
}
