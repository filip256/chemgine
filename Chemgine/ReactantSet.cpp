#include "ReactantSet.hpp"

ReactantSet::ReactantSet(
	const ReactantSet& other,
	const Ref<Mixture> newContainer
) noexcept :
	container(newContainer),
	content(other.content)
{}

ReactantSet::ReactantSet(const Ref<Mixture> container) noexcept:
	container(container)
{}

ReactantSet::ReactantSet(const std::vector<Molecule>& content) noexcept:
	container(Ref<Mixture>::nullRef)
{
	for (size_t i = 0; i < content.size(); ++i)
		add(Reactant(content[i], LayerType::UNKNOWN, 1.0));
}

ReactantSet::ReactantSet(const std::vector<Reactant>& content) noexcept:
	container(content.empty() ? Ref<Mixture>::nullRef : content.front().getContainer())
{
	for (size_t i = 0; i < content.size(); ++i)
		add(content[i].mutate(1.0));
}

size_t ReactantSet::size() const
{
	return content.size();
}

void ReactantSet::reserve(const size_t size)
{
	content.reserve(size);
}

bool ReactantSet::contains(const Reactant& reactant) const
{
	return content.contains(reactant);
}

void ReactantSet::add(const Reactant& reactant)
{
	const auto& temp = content.find(reactant);
	if (temp != content.end())
	{
		temp->amount += reactant.amount;
	}

	content.emplace(reactant.molecule, reactant.layer, reactant.amount, container).first;
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

std::unordered_set<Reactant>::const_iterator ReactantSet::erase(
	const std::unordered_set<Reactant>::const_iterator it)
{
	return content.erase(it);
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

bool ReactantSet::equals(const ReactantSet& other, const Amount<>::StorageType epsilon) const
{
	for (const auto& r : content)
		if (r.amount.equals(other.getAmountOf(r), epsilon) == false)
			return false;
	return true;
}

bool ReactantSet::operator==(const ReactantSet& other) const
{
	for (const auto& r : content)
		if (r.amount != other.getAmountOf(r))
			return false;
	return true;
}

bool ReactantSet::operator!=(const ReactantSet& other) const
{
	for (const auto& r : content)
		if (r.amount != other.getAmountOf(r))
			return true;
	return false;
}

ReactantSet ReactantSet::makeCopy(const Ref<Mixture> newContainer) const
{
	return ReactantSet(*this, newContainer);
}