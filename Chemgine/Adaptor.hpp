#pragma once

#include "DrawableComponent.hpp"
#include "AdaptorData.hpp"

class Adaptor : public DrawableComponent
{
public:
	Adaptor(const LabwareIdType id) noexcept;

	const AdaptorData& getData() const override final;

	Adaptor* clone() const override final;
};