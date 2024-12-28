#pragma once

#include "DrawableLabwareData.hpp"
#include "ShapeFillTexture.hpp"
#include "Units.hpp"

class BaseContainerLabwareData : public DrawableLabwareData
{
protected:
	using DrawableLabwareData::DrawableLabwareData;

public:
	virtual Quantity<Liter> getVolume() const = 0;
	virtual const ShapeFillTexture& getFillTexture() const = 0;
};
