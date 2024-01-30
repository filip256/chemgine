#include "Reactor.hpp"
#include "DataStore.hpp"
#include "Query.hpp"
#include "Logger.hpp"

DataStoreAccessor Reactor::dataAccessor = DataStoreAccessor();

Reactor::Reactor(
	SingleLayerMixture<LayerType::GASEOUS>& atmosphere,
	const Amount<Unit::LITER> maxVolume,
	Mixture* overflowTarget
) noexcept :
	MultiLayerMixture(atmosphere, maxVolume, overflowTarget)
{
	dataAccessor.crashIfUninitialized();
	temperatureSpeedEstimator = &dataAccessor.get().estimators.at(
		static_cast<EstimatorIdType>(Estimators::TEMP_TO_REL_RSPEED));
	concentrationSpeedEstimator = &dataAccessor.get().estimators.at(
		static_cast<EstimatorIdType>(Estimators::MCONC_TO_REL_RSPEED));
}

Reactor::Reactor(
	SingleLayerMixture<LayerType::GASEOUS>& atmosphere,
	const Amount<Unit::LITER> maxVolume
) noexcept:
	Reactor(atmosphere, maxVolume, &atmosphere)
{}

void Reactor::setDataStore(const DataStore& dataStore)
{
	dataAccessor.set(dataStore);
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
			MultiLayerMixture::add(Reactant(i.molecule, i.layer, i.amount * speedCoef.asStd() * -1, *this));
		for (const auto& i : r.getProducts())
			MultiLayerMixture::add(Reactant(i.molecule, findLayerFor(i), i.amount * speedCoef.asStd(), *this));

		MultiLayerMixture::add(r.getData().reactionEnergy.to<Unit::JOULE>(speedCoef), r.getReactants().any().layer);
	}
}

void Reactor::consumePotentialEnergy()
{
	for (auto& l : layers)
	{
		if (l.second.potentialEnergy != 0.0)
		{
			l.second.temperature += l.second.potentialEnergy.to<Unit::CELSIUS>(getLayerHeatCapacity(l.first), l.second.moles);
			l.second.potentialEnergy = 0.0;
		}
	}
}

void Reactor::add(const Molecule& molecule, const Amount<Unit::MOLE> amount)
{
	auto r = Reactant(molecule, LayerType::UNKNOWN, amount, *this);
	r.layer = findLayerFor(r);
	MultiLayerMixture::add(r);
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

void Reactor::tick()
{
	removeNegligibles();
	findNewReactions();
	runReactions(1.0);
	consumePotentialEnergy();
	checkOverflow();
}