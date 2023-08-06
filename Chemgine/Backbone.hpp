#pragma once

#include "CompositeComponent.hpp"
#include "BackboneData.hpp"

class Backbone : public CompositeComponent
{
public:
	Backbone(const ComponentIdType id);

	const BackboneData& data() const override final;

	const BaseComponent* getComponent(const c_size idx) const override final;

	uint8_t getPrecedence() const override final;

	std::unordered_map<ComponentIdType, c_size> getComponentCountMap() const override final;
};