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
	pressure(pressure),
	layers({ {LayerType::GASEOUS, LayerProperties(temperature) }})
{
	dataAccessor.crashIfUninitialized();
	temperatureSpeedEstimator = &dataAccessor.get().estimators.at(
		static_cast<EstimatorIdType>(Estimators::TEMP_TO_REL_RSPEED));
	concentrationSpeedEstimator = &dataAccessor.get().estimators.at(
		static_cast<EstimatorIdType>(Estimators::MCONC_TO_REL_RSPEED));
}

void Reactor::setDataStore(const DataStore& dataStore)
{
	dataAccessor.set(dataStore);
}

bool Reactor::tryCreateLayer(const LayerType layer)
{
	if (isRealLayer(layer) == false || layers.contains(layer))
		return false;

	layers.emplace(std::make_pair(layer, 0.0));
	return true;
}

void Reactor::addToLayer(const Reactant& reactant, const uint8_t revert)
{
	tryCreateLayer(reactant.layer);

	totalMoles += reactant.amount * revert;
	layers[reactant.layer].moles += reactant.amount * revert;

	const auto mass = reactant.getMass() * revert;
	totalMass += mass;
	layers[reactant.layer].mass += mass;

	const auto vol = reactant.getVolume() * revert;
	totalVolume += vol;
	layers[reactant.layer].volume += vol;
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

void Reactor::runReactions(const Amount<Unit::SECOND> timespan)
{
	for (const auto& r : cachedReactions)
	{
		auto speedCoef =
			r.getData().baseSpeed.to<Unit::MOLE>(timespan) *
			totalVolume.asStd() *
			temperatureSpeedEstimator->get((r.getReactantTemperature() - r.getData().baseTemperature).asStd()) *
			concentrationSpeedEstimator->get((getAmountOf(r.getReactants()) / totalMoles).asStd());
		
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
			add(Reactant(i.molecule, i.layer, i.amount * speedCoef.asStd() * -1, *this));
		for (const auto& i : r.getProducts())
			add(Reactant(i.molecule, findLayerFor(i), i.amount * speedCoef.asStd(), *this));

		add(r.getData().reactionEnergy.to<Unit::JOULE>(speedCoef), r.getReactants().any().layer);
	}
}

void Reactor::consumePotentialEnergy()
{
	for (auto& l : layers)
	{
		l.second.temperature += l.second.potentialEnergy.to<Unit::CELSIUS>(getLayerHeatCapacity(l.first), l.second.moles);
	}
}

LayerType Reactor::getLayerAbove(LayerType layer) const
{
	while(layer > LayerType::GASEOUS)
	{
		--layer;
		if (hasLayer(layer))
			return layer;
	}

	return LayerType::NONE;
}

LayerType Reactor::getLayerBelow(LayerType layer) const
{
	while (layer < LayerType::SOLID)
	{
		++layer;
		if (hasLayer(layer))
			return layer;
	}

	return LayerType::NONE;
}

void Reactor::add(const Amount<Unit::JOULE> heat, const LayerType layer)
{
	layers.at(layer).potentialEnergy += heat;
}

void Reactor::add(Reactor& other)
{
	//for (auto const& r : other.content)
	//{
	//	auto const it = this->content.find(r);
	//	if (it == this->content.end())
	//		this->content.emplace(r);
	//	else
	//		it->amount += r.amount;
	//}
}

void Reactor::add(Reactor& other, const double ratio)
{
	//if (ratio >= 1.0)
	//{
	//	this->add(other);
	//	return;
	//}

	//for (auto& r : other.content)
	//{
	//	auto const it = this->content.find(r);
	//	if (it == this->content.end())
	//		this->content.emplace(Reactant(r.molecule, r.layer, r.amount * ratio));
	//	else
	//		it->amount += r.amount * ratio;
	//	r.amount -= r.amount * ratio;
	//}
}

void Reactor::add(const Reactant& reactant)
{
	const auto& temp = content.emplace(reactant);
	if (temp.second == false)
		temp.first->amount += reactant.amount;
	else
	{
		temp.first->setContainer(*this);
		temp.first->isNew = true;
	}

	addToLayer(reactant);
}

void Reactor::add(const Molecule& molecule, const Amount<Unit::MOLE> amount)
{
	auto r = Reactant(molecule, LayerType::UNKNOWN, amount, *this);
	r.layer = findLayerFor(r);
	add(r);
}

void Reactor::add(const Amount<Unit::JOULE> heat)
{
	const auto lA = getLayerAbove(LayerType::SOLID);
	const auto& lS = layers.find(LayerType::SOLID);
	if (lS != layers.end())
	{
		if (lA == LayerType::NONE)
		{
			lS->second.potentialEnergy += heat * 0.5;
			layers.at(lA).potentialEnergy += heat * 0.5;
			return;
		}

		lS->second.potentialEnergy += heat;
		return;
	}

	layers.at(lA).potentialEnergy += heat;
}

LayerType Reactor::findLayerFor(const Reactant& reactant) const
{
	return LayerType::POLAR;
}

Amount<Unit::JOULE_PER_MOLE_CELSIUS> Reactor::getLayerHeatCapacity(const LayerType layer) const
{
	Amount<Unit::JOULE_PER_MOLE_CELSIUS> hC = 0.0;
	for (const auto& r : content)
	{
		if (r.layer == layer)
			hC += r.getHeatCapacity() * r.getMass().asStd();
	}

	return hC / layers.at(layer).getMass().asStd();
}

Amount<Unit::JOULE_PER_MOLE> Reactor::getLayerKineticEnergy(const LayerType layer) const
{
	return getLayerHeatCapacity(layer).to<Unit::JOULE_PER_MOLE>(layers.at(layer).temperature);
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

Amount<Unit::TORR> Reactor::getPressure() const
{
	return pressure;
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

bool Reactor::hasLayer(const LayerType layer) const
{
	const auto& l = layers.find(layer);
	return l != layers.end() && l->second.isEmpty() == false;
}

void Reactor::tick()
{
	removeNegligibles();
	findNewReactions();
	runReactions(1.0);
	consumePotentialEnergy();
}