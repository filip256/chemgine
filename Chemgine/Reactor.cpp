#include "Reactor.hpp"
#include "DataStore.hpp"
#include "PairHash.hpp"
#include "Constants.hpp"
#include "Query.hpp"

Reactor::Reactant::Reactant(
	const Molecule& molecule,
	const LayerType layer,
	const Amount<Unit::MOLE> amount
) noexcept :
	molecule(molecule),
	layer(layer),
	amount(amount),
	isNew(true)
{}

bool Reactor::Reactant::operator== (const Reactant& other) const
{
	return this->layer == other.layer && 
		   this->molecule.getId() == other.molecule.getId();
}

bool Reactor::Reactant::operator!= (const Reactant& other) const
{
	return this->layer != other.layer &&
		this->molecule.getId() != other.molecule.getId();
}


size_t Reactor::ReactantHash::operator() (const Reactor::Reactant& reactant) const
{
	return PairHash()(std::make_pair(reactant.molecule.getId(), reactant.layer));
}



DataStoreAccessor Reactor::dataAccessor = DataStoreAccessor();

Reactor::Reactor() noexcept
{
	dataAccessor.crashIfUninitialized();
}

void Reactor::setDataStore(const DataStore& dataStore)
{
	dataAccessor.set(dataStore);
}

void Reactor::removeNegligibles()
{
	for (auto const& r : content)
	{
		if (r.amount < Constants::MOLAR_EXISTANCE_THRESHOLD)
			content.erase(r);
	}
}

void Reactor::checkReactions()
{
	for (const auto& r1 : content)
		dataAccessor.get().reactions.findOccuringReactions(std::vector<Molecule>{ r1.molecule });

	for (const auto& r1 : content)
		for (const auto& r2 : content)
			dataAccessor.get().reactions.findOccuringReactions(std::vector<Molecule>{ r1.molecule, r2.molecule });
}

void Reactor::add(Reactor& other)
{
	for (auto const& r : other.content)
	{
		auto const it = this->content.find(r);
		if (it == this->content.end())
			this->content.emplace(r);
		else
			it->amount += r.amount;
	}
}

void Reactor::add(Reactor& other, const double ratio)
{
	if (ratio >= 1.0)
	{
		this->add(other);
		return;
	}

	for (auto& r : other.content)
	{
		auto const it = this->content.find(r);
		if (it == this->content.end())
			this->content.emplace(Reactor::Reactant(r.molecule, r.layer, r.amount * ratio));
		else
			it->amount += r.amount * ratio;
		r.amount -= r.amount * ratio;
	}
}

void Reactor::add(const Molecule& molecule, const Amount<Unit::MOLE> amount)
{
	content.emplace(molecule, LayerType::POLAR, amount);
}

void Reactor::tick()
{
	removeNegligibles();
	checkReactions();
}