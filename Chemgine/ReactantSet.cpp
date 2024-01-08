#include "ReactantSet.hpp"


ReactantSet::ReactantSet(const std::vector<Molecule>& content) noexcept
{
	for (size_t i = 0; i < content.size(); ++i)
		add(Reactant(content[i], LayerType::UNKNOWN, 1.0));
}

ReactantSet::ReactantSet(const std::vector<Reactant>& content) noexcept
{
	for (size_t i = 0; i < content.size(); ++i)
		add(Reactant(content[i].molecule, content[i].layer, 1.0));
}

bool ReactantSet::contains(const Reactant& reactant) const
{
	return content.contains(reactant);
}

void ReactantSet::add(const Reactant& reactant)
{
	auto const it = this->content.find(reactant);
	if (it == this->content.end())
		this->content.emplace(reactant);
	else
		it->amount += reactant.amount;
}


std::unordered_set<Reactant, ReactantHash>::const_iterator ReactantSet::begin() const
{
	return content.begin();
}

std::unordered_set<Reactant, ReactantHash>::const_iterator ReactantSet::end() const
{
	return content.end();
}