#pragma once

#include "CompositeComponent.hpp"
#include "FunctionalGroupData.hpp"

class FunctionalGroup : public CompositeComponent
{
protected:
	ComponentIdType id;

public:
	FunctionalGroup(const ComponentIdType id);

	const FunctionalGroupData& data() const override final;


};