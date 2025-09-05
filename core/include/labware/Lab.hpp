#pragma once

#include "labware/LabwareSystem.hpp"
#include "mixtures/kinds/Atmosphere.hpp"

#include <SFML/Graphics/Drawable.hpp>

class Lab : public sf::Drawable
{
private:
    std::unique_ptr<Atmosphere> atmosphere;
    mutable sf::RectangleShape  atmosphereOverlay;
    std::vector<LabwareSystem>  systems;

public:
    Lab() noexcept;
    Lab(std::unique_ptr<Atmosphere>&& atmosphere) noexcept;
    Lab(const Lab&) = delete;
    Lab(Lab&&)      = default;

    void add(LabwareSystem&& system);

    template <typename CompT, typename... Args>
    CompT& add(Args&&... args);

    void removeEmptySystems();

    size_t               getSystemCount() const;
    const LabwareSystem& getSystem(const size_t idx) const;
    LabwareSystem&       getSystem(const size_t idx);
    const Atmosphere&    getAtmosphere() const;
    Atmosphere&          getAtmosphere();

    size_t                    getSystemAt(const sf::Vector2f& point) const;
    std::pair<size_t, l_size> getSystemComponentAt(const sf::Vector2f& point) const;
    size_t                    anyIntersects(const size_t targetIdx) const;

    bool tryConnect(const size_t targetIdx, const float_s maxSqDistance);
    bool tryDisconnect(const sf::Vector2f& point);

    using LabSystemsConstIterator = std::vector<LabwareSystem>::const_iterator;
    LabSystemsConstIterator getSystemsBegin() const;
    LabSystemsConstIterator getSystemsEnd() const;

    using LabSystemsIterator = std::vector<LabwareSystem>::iterator;
    LabSystemsIterator getSystemsBegin();
    LabSystemsIterator getSystemsEnd();

    void tick(const Amount<Unit::SECOND> timespan);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;

    static constexpr const size_t npos = static_cast<size_t>(-1);
};

template <typename CompT, typename... Args>
CompT& Lab::add(Args&&... args)
{
    static_assert(
        std::is_base_of_v<LabwareComponentBase, CompT>, "Lab: CompT must be a LabwareComponentBase derived type.");

    std::unique_ptr<CompT> ptr;
    if constexpr (std::is_constructible_v<CompT, Args...>)
        ptr = std::make_unique<CompT>(std::forward<Args>(args)...);
    else
        ptr = std::make_unique<CompT>(std::forward<Args>(args)..., *atmosphere);

    auto& ref = *ptr.get();
    systems.emplace_back(std::move(ptr), *this);
    return ref;
}
