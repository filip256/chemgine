#include "ForwardingContainer.hpp"

ForwardingContainer::ForwardingContainer(
	std::initializer_list<ForwardingRule> forwardingRules,
	Ref<BaseContainer> defaultTarget
) noexcept :
	forwardingRules(forwardingRules),
	defaultTarget(defaultTarget)
{}

ForwardingContainer::ForwardingContainer(
	const std::vector<ForwardingRule>& forwardingRules,
	Ref<BaseContainer> defaultTarget
) noexcept :
	forwardingRules(forwardingRules),
	defaultTarget(defaultTarget)
{}

void ForwardingContainer::add(const Reactant& reactant)
{
	for (size_t i = 0; i < forwardingRules.size(); ++i)
		if (forwardingRules[i].predicate(reactant))
			forwardingRules[i].target->add(reactant);
}

void ForwardingContainer::add(Amount<Unit::JOULE> energy)
{
	energy /= forwardingRules.size();
	for (size_t i = 0; i < forwardingRules.size(); ++i)
		forwardingRules[i].target->add(energy);
}

void ForwardingContainer::addRule(ForwardingRule rule)
{
	forwardingRules.push_back(rule);
}

ForwardingRule ForwardingContainer::getRule(const size_t idx) const
{
	return forwardingRules[idx];
}

const std::vector<ForwardingRule>& ForwardingContainer::getRules() const
{
	return forwardingRules;
}
