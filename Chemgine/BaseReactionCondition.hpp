#pragma once

class BaseReactionCondition
{
protected:
	BaseReactionCondition() = default;
	virtual ~BaseReactionCondition() = default;

public:
	virtual float fulfillmentLevel() = 0;
};
