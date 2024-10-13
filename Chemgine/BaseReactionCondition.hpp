#pragma once

class BaseReactionCondition
{
protected:
	BaseReactionCondition() = default;
	virtual ~BaseReactionCondition() = default;

public:
	virtual float_n fulfillmentLevel() = 0;
};
