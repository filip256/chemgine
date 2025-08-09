#pragma once

#include "labware/data/DrawableLabwareData.hpp"
#include "graphics/ShapeFillTexture.hpp"
#include "data/values/Amount.hpp"

class BaseContainerLabwareData : public DrawableLabwareData
{
protected:
	using DrawableLabwareData::DrawableLabwareData;

public:
	virtual Amount<Unit::LITER> getVolume() const = 0;
	virtual const ShapeFillTexture& getFillTexture() const = 0;
};
