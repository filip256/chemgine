#pragma once

#include "CompositeComponent.hpp"
#include "BackboneData.hpp"

class Backbone : public CompositeComponent
{
public:
	Backbone(const ComponentIdType id);

	const BackboneData& data() const override final;

	const BaseComponent* getComponent(const size_t idx) const override final;

	uint8_t getPrecedence() const override final;

	std::unordered_map<ComponentIdType, size_t> getComponentCountMap() const override final;
};