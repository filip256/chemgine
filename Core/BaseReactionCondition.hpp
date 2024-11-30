#pragma once

class BaseReactionCondition
{
protected:
	BaseReactionCondition() = default;
	virtual ~BaseReactionCondition() = default;

public:
	virtual float_s fulfillmentLevel() = 0;
};
