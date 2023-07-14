#pragma once

class BaseReactionCondition
{
protected:
	BaseReactionCondition() = default;

public:
	virtual float fulfillmentLevel() = 0;
};