#pragma once

#include "data/values/Amount.hpp"
#include "graphics/ShapeFillTexture.hpp"
#include "labware/data/DrawableLabwareData.hpp"

class BaseContainerLabwareData : public DrawableLabwareData
{
protected:
    using DrawableLabwareData::DrawableLabwareData;

public:
    virtual Amount<Unit::LITER>     getVolume() const      = 0;
    virtual const ShapeFillTexture& getFillTexture() const = 0;
};
