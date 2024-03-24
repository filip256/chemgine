#pragma once

#include "Atmosphere.hpp"
#include "LabwareSystem.hpp"
#include "SFML/Graphics/Drawable.hpp"

class Lab : public sf::Drawable
{
private:
	Atmosphere atmosphere;
	mutable sf::RectangleShape atmosphereOverlay;
	std::vector<LabwareSystem> systems;

public:
	Lab() noexcept;
	Lab(Atmosphere&& atmosphere) noexcept;
	Lab(const Lab&) = delete;
	Lab(Lab&&) = default;

	void add(LabwareSystem&& system);

	template <typename CompT, typename... Args, typename = std::enable_if_t<
		std::is_base_of_v<BaseLabwareComponent, CompT> && (
			std::is_constructible_v<CompT, Args..., Atmosphere&> ||
			std::is_constructible_v<CompT, Args...>
			)>>
	CompT& add(Args&&... args);

	void removeEmptySystems();

	size_t getSystemCount() const;
	const LabwareSystem& getSystem(const size_t idx) const;
	LabwareSystem& getSystem(const size_t idx);

	size_t getSystemAt(const sf::Vector2f& point) const;
	std::pair<size_t, l_size> getSystemComponentAt(const sf::Vector2f& point) const;
	size_t anyIntersects(const size_t targetIdx) const;

	bool tryConnect(const size_t targetIdx, const float maxSqDistance);
	bool tryDissconnect(const sf::Vector2f& point);

	using LabSystemsConstIterator = std::vector<LabwareSystem>::const_iterator;
	LabSystemsConstIterator getSystemsBegin() const;
	LabSystemsConstIterator getSystemsEnd() const;

	using LabSystemsIterator = std::vector<LabwareSystem>::iterator;
	LabSystemsIterator getSystemsBegin();
	LabSystemsIterator getSystemsEnd();

	void tick(const Amount<Unit::SECOND> timespan);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;

	constexpr static const size_t npos = static_cast<size_t>(-1);
};


template <typename CompT, typename... Args, typename>
CompT& Lab::add(Args&&... args)
{
	CompT* temp;
	if constexpr (std::is_constructible_v<CompT, Args...>)
		temp = new CompT(std::forward<Args>(args)...);
	else
		temp = new CompT(std::forward<Args>(args)..., atmosphere);

	systems.emplace_back(temp);
	return *temp;
}
