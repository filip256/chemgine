#include "ReactantSet.hpp"


ReactantSet::ReactantSet(const std::vector<Molecule>& content) noexcept
{
	for (size_t i = 0; i < content.size(); ++i)
		add(Reactant(content[i], LayerType::UNKNOWN, 1.0));
}

ReactantSet::ReactantSet(const std::vector<Reactant>& content) noexcept
{
	for (size_t i = 0; i < content.size(); ++i)
		add(Reactant(content[i].molecule, content[i].layer, 1.0, *content[i].getContainer()));
}

bool ReactantSet::contains(const Reactant& reactant) const
{
	return content.contains(reactant);
}

void ReactantSet::add(const Reactant& reactant)
{
	const auto& temp = content.emplace(reactant);
	if (temp.second == false)
		temp.first->amount += reactant.amount;
	else
		temp.first->markAsNew();
}

const Reactant& ReactantSet::any() const
{
	return *content.begin();
}

Amount<Unit::MOLE> ReactantSet::getAmountOf(const Reactant& reactant) const
{
	const auto it = content.find(reactant);
	return it == content.end() ? Amount<Unit::MOLE>(0.0) : it->amount;
}

Amount<Unit::MOLE> ReactantSet::getAmountOf(const ReactantSet& reactantSet) const
{
	Amount<Unit::MOLE> s = 0.0;
	for (const auto& r : reactantSet)
		s += getAmountOf(r);
	return s;
}

void ReactantSet::erase(bool (*predicate)(const Reactant&))
{
	std::erase_if(content, predicate);
}

std::unordered_set<Reactant>::const_iterator ReactantSet::begin() const
{
	return content.begin();
}

std::unordered_set<Reactant>::const_iterator ReactantSet::end() const
{
	return content.end();
}