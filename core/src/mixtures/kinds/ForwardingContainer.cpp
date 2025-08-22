#include "mixtures/kinds/ForwardingContainer.hpp"

ForwardingContainer::ForwardingContainer(
    std::initializer_list<ForwardingRule> forwardingRules,
    Ref<ContainerBase>                    defaultTarget) noexcept :
    defaultTarget(defaultTarget),
    forwardingRules(forwardingRules)
{}

ForwardingContainer::ForwardingContainer(
    const std::vector<ForwardingRule>& forwardingRules, Ref<ContainerBase> defaultTarget) noexcept :
    defaultTarget(defaultTarget),
    forwardingRules(forwardingRules)
{}

void ForwardingContainer::add(const Reactant& reactant)
{
    for (size_t i = 0; i < forwardingRules.size(); ++i)
        if (forwardingRules[i].predicate(reactant))
            forwardingRules[i].target->add(reactant);
}

void ForwardingContainer::addEnergy(const Amount<Unit::JOULE> energy)
{
    const auto splitEnergy = energy / static_cast<Amount<>::StorageType>(forwardingRules.size());
    for (size_t i = 0; i < forwardingRules.size(); ++i)
        forwardingRules[i].target->addEnergy(splitEnergy);
}

void ForwardingContainer::addRule(ForwardingRule rule) { forwardingRules.push_back(rule); }

ForwardingRule ForwardingContainer::getRule(const size_t idx) const { return forwardingRules[idx]; }

const std::vector<ForwardingRule>& ForwardingContainer::getRules() const { return forwardingRules; }
