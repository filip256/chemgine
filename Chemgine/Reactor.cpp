#include "Reactor.hpp"
#include "DataStore.hpp"
#include "Constants.hpp"
#include "Query.hpp"
#include "Logger.hpp"

DataStoreAccessor Reactor::dataAccessor = DataStoreAccessor();

Reactor::Reactor(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) noexcept:
	temperature(temperature),
	pressure(pressure)
{
	dataAccessor.crashIfUninitialized();
}

void Reactor::setDataStore(const DataStore& dataStore)
{
	dataAccessor.set(dataStore);
}

void Reactor::removeNegligibles()
{
	std::erase_if(content, [](const auto& r) { return r.amount < Constants::MOLAR_EXISTANCE_THRESHOLD; });
}

void Reactor::findNewReactions()
{
	for (const auto& r1 : content)
		if (r1.isNew)
		{
			cachedReactions.merge(std::move(
				dataAccessor.get().reactions.findOccuringReactions(std::vector<Molecule>{ r1.molecule })
			));
		}

	for (const auto& r1 : content)
		for (const auto& r2 : content)
		{
			if (r1.isNew || r2.isNew)
				cachedReactions.merge(std::move(
					dataAccessor.get().reactions.findOccuringReactions(std::vector<Molecule>{ r1.molecule, r2.molecule })
				));
		}

	for (const auto& r : content)
		r.isNew = false;
}

void Reactor::runReactions()
{
	const auto& reactionSpeedApproximator = dataAccessor.get().approximators.at(101);
	for (const auto& r : cachedReactions)
	{
		double speedCoef = 
			r.getData().baseSpeed.asStd() *
			reactionSpeedApproximator.execute((temperature - r.getData().baseTemperature).asStd());
		
		// if there isn't enough of a reactant, adjust the speed coefficient
		for (const auto& i : r.getReactants())
		{
			const auto a = getAmountOf(i);
			if (a < i.amount * speedCoef)
				speedCoef = (a / i.amount).asStd() * 0.8; // 0.8 = low concentration speed loss factor
		}

		for (const auto& i : r.getReactants())
			add(Reactant(i.molecule, i.layer, i.amount * speedCoef * -1));
		for (const auto& i : r.getProducts())
			add(Reactant(i.molecule, i.layer, i.amount * speedCoef));
	}
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
			this->content.emplace(Reactant(r.molecule, r.layer, r.amount * ratio));
		else
			it->amount += r.amount * ratio;
		r.amount -= r.amount * ratio;
	}
}

void Reactor::add(const Reactant& reactant)
{
	const auto temp = content.emplace(reactant);
	if (temp.second == false)
		temp.first->amount += reactant.amount;
	else
		temp.first->isNew = true;
}

void Reactor::add(const Molecule& molecule, const Amount<Unit::MOLE> amount)
{
	const auto temp = content.emplace(molecule, LayerType::POLAR, amount);
	if (temp.second == false)
		temp.first->amount += amount;
	else
		temp.first->isNew = true;
}

Amount<Unit::MOLE> Reactor::getAmountOf(const Reactant& reactant) const
{
	const auto it = content.find(reactant);
	return it == content.end() ? Amount<Unit::MOLE>(0.0) : it->amount;
}

void Reactor::tick()
{
	removeNegligibles();
	findNewReactions();
	runReactions();
}