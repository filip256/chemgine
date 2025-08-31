#pragma once

#include "ContainerBase.hpp"
#include "mixtures/kinds/DumpContainer.hpp"
#include "structs/Ref.hpp"

#include <vector>

class ForwardingRule
{
public:
    bool               (*predicate)(const Reactant&);
    Ref<ContainerBase> target;
};

class ForwardingContainer : public ContainerBase
{
private:
    Ref<ContainerBase>          defaultTarget = DumpContainer::GlobalDumpContainer;
    std::vector<ForwardingRule> forwardingRules;

public:
    ForwardingContainer(
        std::initializer_list<ForwardingRule> forwardingRules, Ref<ContainerBase> defaultTarget) noexcept;

    ForwardingContainer(const std::vector<ForwardingRule>& forwardingRules, Ref<ContainerBase> defaultTarget) noexcept;

    ForwardingContainer(const ForwardingContainer&) = delete;
    ForwardingContainer(ForwardingContainer&&)      = default;

    void add(const Reactant& reactant) override final;
    void addEnergy(const Amount<Unit::JOULE> energy) override final;

    void                               addRule(ForwardingRule rule);
    ForwardingRule                     getRule(const size_t idx) const;
    const std::vector<ForwardingRule>& getRules() const;
};
