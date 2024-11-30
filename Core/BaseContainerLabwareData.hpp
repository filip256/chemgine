#pragma once

#include "DrawableLabwareData.hpp"
#include "ShapeFillTexture.hpp"
#include "Amount.hpp"

class BaseContainerLabwareData : public DrawableLabwareData
{
protected:
	using DrawableLabwareData::DrawableLabwareData;

public:
	virtual Amount<Unit::LITER> getVolume() const = 0;
	virtual const ShapeFillTexture& getFillTexture() const = 0;
};
