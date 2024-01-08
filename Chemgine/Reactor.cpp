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
	temperatureSpeedApproximator = &dataAccessor.get().approximators.at(101);
	concentrationSpeedApproximator = &dataAccessor.get().approximators.at(102);
}

void Reactor::setDataStore(const DataStore& dataStore)
{
	dataAccessor.set(dataStore);
}

void Reactor::addToLayer(const Reactant& reactant, const uint8_t revert)
{
	totalMoles += reactant.amount * revert;
	layerProperties[toIndex(reactant.layer)].moles += reactant.amount * revert;

	const auto mass = reactant.getMass() * revert;
	totalMass += mass;
	layerProperties[toIndex(reactant.layer)].mass += mass;

	const auto vol = reactant.getVolumeAt(temperature, pressure) * revert;
	totalVolume += vol;
	layerProperties[toIndex(reactant.layer)].volume += vol;
}

void Reactor::removeFromLayer(const Reactant& reactant)
{
	addToLayer(reactant, -1);
}

void Reactor::removeNegligibles()
{
	for (const auto& r : content)
		if (isRealLayer(r.layer) && r.amount < Constants::MOLAR_EXISTANCE_THRESHOLD)
			removeFromLayer(r);

	std::erase_if(content, [](const auto& r) { return r.amount < Constants::MOLAR_EXISTANCE_THRESHOLD; });
}

void Reactor::findNewReactions()
{
	for (const auto& r1 : content)
		if (r1.isNew)
		{
			cachedReactions.merge(std::move(
				dataAccessor.get().reactions.findOccuringReactions(std::vector<Reactant>{ r1 })
			));
		}

	for (const auto& r1 : content)
		for (const auto& r2 : content)
		{
			if (r1.isNew || r2.isNew)
				cachedReactions.merge(std::move(
					dataAccessor.get().reactions.findOccuringReactions(std::vector<Reactant>{ r1, r2 })
				));
		}

	for (const auto& r : content)
		r.isNew = false;
}

void Reactor::runReactions()
{
	for (const auto& r : cachedReactions)
	{
		auto speedCoef =
			r.getData().baseSpeed *
			totalVolume.asStd() *
			temperatureSpeedApproximator->get((temperature - r.getData().baseTemperature).asStd()) *
			concentrationSpeedApproximator->get((getAmountOf(r.getReactants()) / totalMoles).asStd());
		
		if (speedCoef == 0)
			continue;

		// if there isn't enough of a reactant, adjust the speed coefficient
		for (const auto& i : r.getReactants())
		{
			const auto a = getAmountOf(i);
			if (a < i.amount * speedCoef.asStd())
				speedCoef = (a / i.amount).asStd();
		}

		if (speedCoef == 0)
			continue;

		for (const auto& i : r.getReactants())
			add(Reactant(i.molecule, i.layer, i.amount * speedCoef.asStd() * -1));
		for (const auto& i : r.getProducts())
			add(Reactant(i.molecule, findLayerFor(i), i.amount * speedCoef.asStd()));
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
	if (isRealLayer(reactant.layer))
		addToLayer(reactant);

	const auto temp = content.emplace(reactant);
	if (temp.second == false)
		temp.first->amount += reactant.amount;
	else
		temp.first->isNew = true;
}

void Reactor::add(const Molecule& molecule, const Amount<Unit::MOLE> amount)
{
	add(Reactant(molecule, LayerType::UNKNOWN, amount));
}

LayerType Reactor::findLayerFor(const Reactant& reactant) const
{
	return LayerType::POLAR;
}

Amount<Unit::MOLE> Reactor::getAmountOf(const Reactant& reactant) const
{
	const auto it = content.find(reactant);
	return it == content.end() ? Amount<Unit::MOLE>(0.0) : it->amount;
}

Amount<Unit::MOLE> Reactor::getAmountOf(const ReactantSet& reactantSet) const
{
	Amount<Unit::MOLE> s = 0.0;
	for (const auto& r : reactantSet)
		s += getAmountOf(r);
	return s;
}

Amount<Unit::MOLE> Reactor::getTotalMoles() const
{
	return totalMoles;
}

Amount<Unit::GRAM> Reactor::getTotalMass() const
{
	return totalMass;
}

Amount<Unit::LITER> Reactor::getTotalVolume() const
{
	return totalVolume;
}

void Reactor::tick()
{
	removeNegligibles();
	findNewReactions();
	runReactions();
}