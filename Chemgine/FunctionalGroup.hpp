#pragma once

#include "CompositeComponent.hpp"
#include "FunctionalGroupData.hpp"

class FunctionalGroup : public CompositeComponent
{
public:
	FunctionalGroup(const ComponentIdType id);

	const FunctionalGroupData& data() const override final;

	const BaseComponent* getComponent(const c_size idx) const override final;

	uint8_t getPrecedence() const override final;

	std::unordered_map<ComponentIdType, c_size> getComponentCountMap() const override final;
};