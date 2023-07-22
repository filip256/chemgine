#pragma once

#include "CompositeComponent.hpp"
#include "FunctionalGroupData.hpp"

class FunctionalGroup : public CompositeComponent
{
public:
	FunctionalGroup(const ComponentIdType id);

	const FunctionalGroupData& data() const override final;

	const BaseComponent* getComponent(const size_t idx) const override final;

	uint8_t getPrecedence() const override final;

	std::unordered_map<ComponentIdType, size_t> getComponentCountMap() const override final;
};