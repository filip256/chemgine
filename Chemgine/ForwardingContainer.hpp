#pragma once

#include "BaseContainer.hpp"
#include "DumpContainer.hpp"
#include "Ref.hpp"

#include <vector>

class ForwardingRule
{
public:
	bool (*predicate) (const Reactant&);
	Ref<BaseContainer> target;
};

class ForwardingContainer : public BaseContainer
{
private:
	Ref<BaseContainer> defaultTarget = DumpContainer::globalDumpContainer;
	std::vector<ForwardingRule> forwardingRules;

public:
	ForwardingContainer(
		std::initializer_list<ForwardingRule> forwardingRules,
		Ref<BaseContainer> defaultTarget
	) noexcept;

	ForwardingContainer(
		const std::vector<ForwardingRule>& forwardingRules,
		Ref<BaseContainer> defaultTarget
	) noexcept;

	ForwardingContainer(const ForwardingContainer&) = delete;
	ForwardingContainer(ForwardingContainer&&) = default;

	void add(const Reactant& reactant) override final;

	void addRule(ForwardingRule rule);
	ForwardingRule getRule(const size_t idx) const;
	const std::vector<ForwardingRule>& getRules() const;
};