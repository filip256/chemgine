#include "ReactantSet.hpp"

ReactantSet::ReactantSet(
	const ReactantSet& other,
	const Ref<Mixture> newContainer
) noexcept :
	container(newContainer)
{
	reactants.reserve(other.reactants.size());
	add(other);
}

ReactantSet::ReactantSet(const Ref<Mixture> container) noexcept :
	container(container)
{}

ReactantSet::ReactantSet(const std::vector<Molecule>& reactants) noexcept :
	container(nullRef)
{
	for (size_t i = 0; i < reactants.size(); ++i)
		add(Reactant(reactants[i], LayerType::NONE, 1.0));
}

ReactantSet::ReactantSet(const std::vector<Reactant>& reactants) noexcept :
	container(reactants.empty() ? nullRef : reactants.front().getContainer())
{
	for (size_t i = 0; i < reactants.size(); ++i)
		add(reactants[i].mutate(1.0_mol));
}

size_t ReactantSet::size() const
{
	return reactants.size();
}

void ReactantSet::reserve(const size_t size)
{
	reactants.reserve(size);
}

bool ReactantSet::contains(const ReactantId& reactantId) const
{
	return reactants.contains(reactantId);
}

void ReactantSet::add(const Reactant& reactant)
{
	const auto& temp = reactants.find(reactant.getId());
	if (temp != reactants.end())
	{
		temp->second.amount += reactant.amount;
	}

	reactants.emplace(std::make_pair(reactant.getId(), reactant.mutate(container)));
}

void ReactantSet::add(const ReactantSet& other)
{
	for (const auto& [_, r] : other.reactants)
		add(r);
}

const Reactant& ReactantSet::any() const
{
	return reactants.begin()->second;
}

Amount<Unit::MOLE> ReactantSet::getAmountOf(const ReactantId& reactantId) const
{
	const auto it = reactants.find(reactantId);
	return it == reactants.end() ? Amount<Unit::MOLE>(0.0) : it->second.amount;
}

Amount<Unit::MOLE> ReactantSet::getAmountOf(const ReactantSet& reactantSet) const
{
	Amount<Unit::MOLE> s = 0.0;
	for (const auto& r : reactantSet)
		s += getAmountOf(r.first);
	return s;
}

ReactantSet::iterator ReactantSet::erase(const ReactantSet::iterator it)
{
	return reactants.erase(it);
}

void ReactantSet::erase(bool (*predicate)(const ReactantSet::pairT&))
{
	std::erase_if(reactants, predicate);
}

ReactantSet::const_iterator ReactantSet::begin() const
{
	return reactants.cbegin();
}

ReactantSet::iterator ReactantSet::begin()
{
	return reactants.begin();
}

ReactantSet::const_iterator ReactantSet::end() const
{
	return reactants.cend();
}

ReactantSet::iterator ReactantSet::end()
{
	return reactants.end();
}

bool ReactantSet::equals(const ReactantSet& other, const Amount<>::StorageType epsilon) const
{
	for (const auto& [_, r] : reactants)
		if (r.amount.equals(other.getAmountOf(r), epsilon) == false)
			return false;
	return true;
}

bool ReactantSet::operator==(const ReactantSet& other) const
{
	for (const auto& [_, r] : reactants)
		if (r.amount != other.getAmountOf(r))
			return false;
	return true;
}

bool ReactantSet::operator!=(const ReactantSet& other) const
{
	for (const auto& [_, r] : reactants)
		if (r.amount != other.getAmountOf(r))
			return true;
	return false;
}

ReactantSet ReactantSet::makeCopy(const Ref<Mixture> newContainer) const
{
	return ReactantSet(*this, newContainer);
}
