#pragma once

#include "DrawableComponent.hpp"
#include "FlaskData.hpp"

class Flask : public DrawableComponent
{
public:
	Flask(const LabwareId id) noexcept;

	const FlaskData& getData() const override final;

	Flask* clone() const override final;
};