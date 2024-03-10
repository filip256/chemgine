#include "Reactor.hpp"
#include "DataStore.hpp"
#include "Query.hpp"
#include "Logger.hpp"

DataStoreAccessor Reactor::dataAccessor = DataStoreAccessor();

Reactor::Reactor(const Reactor& other) noexcept :
	MultiLayerMixture(other),
	temperatureSpeedEstimator(other.temperatureSpeedEstimator),
	concentrationSpeedEstimator(other.concentrationSpeedEstimator),
	stirSpeed(other.stirSpeed),
	tickMode(other.tickMode)
{
	this->cachedReactions.reserve(other.cachedReactions.size());
	for (const auto& r : other.cachedReactions)
		this->cachedReactions.emplace(std::move(r.makeCopy()));
}

Reactor::Reactor(
	const Ref<Atmosphere> atmosphere,
	const Amount<Unit::LITER> maxVolume,
	const Ref<BaseContainer> overflowTarget
) noexcept :
	MultiLayerMixture(atmosphere, maxVolume, overflowTarget)
{
	dataAccessor.crashIfUninitialized();
	temperatureSpeedEstimator = &dataAccessor.get().estimators.at(
		toId(BuiltinEstimator::TEMP_TO_REL_RSPEED));
	concentrationSpeedEstimator = &dataAccessor.get().estimators.at(
		toId(BuiltinEstimator::MCONC_TO_REL_RSPEED));
}

Reactor::Reactor(
	const Ref<Atmosphere> atmosphere,
	const Amount<Unit::LITER> maxVolume
) noexcept :
	Reactor(atmosphere, maxVolume, atmosphere)
{}

void Reactor::setDataStore(const DataStore& dataStore)
{
	dataAccessor.set(dataStore);
}

double Reactor::getInterLayerReactivityCoefficient(const Reactant& r1, const Reactant& r2) const
{
	if (r1.layer == r2.layer)
	{
		if (isSolidLayer(r1.layer))
			return 0.0001;
		return 1.0;
	}

	if (MultiLayerMixture::areAdjacentLayers(r1.layer, r2.layer) == false)
		return 0.0;

	// TODO: take into account granularity, stirring, bubbling etc.
	if (isSolidLayer(r1.layer))
	{
		if (isLiquidLayer(r2.layer))
			return 0.5;   // S-L
		return 0.01;      // S-G
	}

	if (isLiquidLayer(r1.layer))
	{
		if (isSolidLayer(r2.layer))
			return 0.5;   // L-S
		return 0.1;       // L-G
	}


	if (isLiquidLayer(r2.layer))
		return 0.1;       // G-L
	return 0.01;          // G-S
}

double Reactor::getInterLayerReactivityCoefficient(const ReactantSet& reactants) const
{
	double result = 1.0;
	for (const auto& [_, r1] : reactants)
		for (const auto& [_, r2] : reactants)
		{
			result = std::min(getInterLayerReactivityCoefficient(r1, r2), result);
			if (result == 0.0) // early return, coef is always positive
				return 0.0;
		}
	return result;
}

void Reactor::findNewReactions()
{
	for (const auto& [_, r1] : content)
		if (r1.isNew)
		{
			cachedReactions.merge(std::move(
				dataAccessor.get().reactions.findOccuringReactions(std::vector<Reactant>{ r1 })
			));
		}

	for (const auto& [_, r1] : content)
		for (const auto& [_, r2] : content)
		{
			if (r1.isNew || r2.isNew)
				cachedReactions.merge(std::move(
					dataAccessor.get().reactions.findOccuringReactions(std::vector<Reactant>{ r1, r2 })
				));
		}

	for (auto& [_, r] : content)
		r.isNew = false;
}

void Reactor::runReactions(const Amount<Unit::SECOND> timespan)
{
	for (const auto& r : cachedReactions)
	{
		const auto x = r.getReactantTemperature();

		auto speedCoef =
			r.getData().baseSpeed.to<Unit::MOLE>(timespan) *
			totalVolume.asStd() *
			temperatureSpeedEstimator->get((r.getReactantTemperature() - r.getData().baseTemperature).asStd()) *
			concentrationSpeedEstimator->get((getAmountOf(r.getReactants()) / totalMoles).asStd()) *
			getInterLayerReactivityCoefficient(r.getReactants());
		
		if (speedCoef == 0)
			continue;

		// if there isn't enough of a reactant, adjust the speed coefficient
		for (const auto& [_, i] : r.getReactants())
		{
			const auto a = getAmountOf(i);
			if (a < i.amount * speedCoef)
				speedCoef = a / i.amount;
		}

		if (speedCoef == 0)
			continue;

		for (const auto& [_, i] : r.getReactants())
			MultiLayerMixture::add(i.mutate(-i.amount * speedCoef, *this));
		for (const auto& [_, i] : r.getProducts())
		{
			const auto p = i.mutate(i.amount * speedCoef, *this);
			MultiLayerMixture::add(p.mutate(findLayerFor(p)));
		}

		MultiLayerMixture::add(r.getData().reactionEnergy.to<Unit::JOULE>(speedCoef), r.getReactants().any().layer);
	}
}

void Reactor::runLayerEnergyConduction(const Amount<Unit::SECOND> timespan)
{
	// TODO: find way to determine these based on molecular composition
	const static Amount<Unit::WATT> favourableC = 0.05;    // as relative conductivity
	const static Amount<Unit::WATT> unfavourableC = 0.03;  // as relative conductivity

	for (auto& l : layers)
	{
		if (const auto above = getLayerAbove(l.first); above != LayerType::NONE)
		{
			const auto aboveLayer = layers.at(above);
			const auto diff = (l.second.temperature - aboveLayer.temperature);
			if (diff == 0)
				continue;


			const auto diffE = diff > 0 ?
				// molecules closer to the top ussually have more
				// energy than the layer avg. => favour up conversion
				l.second.getHeatCapacity().to<Unit::JOULE_PER_CELSIUS>(l.second.moles).to<Unit::JOULE>(diff) * favourableC.to<Unit::JOULE>(timespan) :
				l.second.getHeatCapacity().to<Unit::JOULE_PER_CELSIUS>(aboveLayer.moles).to<Unit::JOULE>(diff) * unfavourableC.to<Unit::JOULE>(timespan);

			MultiLayerMixture::add(diffE, above);
			MultiLayerMixture::add(-diffE, l.first);
		}

		if (const auto below = getLayerBelow(l.first); below != LayerType::NONE)
		{
			const auto belowLayer = layers.at(below);
			const auto diff = (l.second.temperature - belowLayer.temperature);
			if (diff == 0)
				continue;

			const auto diffE = diff > 0 ?
				// molecules closer to the bottom ussually have less
				// energy than the layer avg. => favour down conversion
				l.second.getHeatCapacity().to<Unit::JOULE_PER_CELSIUS>(l.second.moles).to<Unit::JOULE>(diff) * unfavourableC.to<Unit::JOULE>(timespan) :
				l.second.getHeatCapacity().to<Unit::JOULE_PER_CELSIUS>(belowLayer.moles).to<Unit::JOULE>(diff) * favourableC.to<Unit::JOULE>(timespan);

			MultiLayerMixture::add(diffE, below);
			MultiLayerMixture::add(-diffE, l.first);
		}
	}
}

void Reactor::consumePotentialEnergy()
{
	for (auto& l : layers)
	{
		l.second.convertTemporaryStateReactants();
		l.second.consumePotentialEnergy();
	}
}

void Reactor::add(const Molecule& molecule, const Amount<Unit::MOLE> amount)
{
	auto r = Reactant(molecule, LayerType::UNKNOWN, amount, *this);
	MultiLayerMixture::add(r.mutate(findLayerFor(r)));
}

void Reactor::add(const Amount<Unit::JOULE> heat)
{
	const auto lA = getLayerAbove(LayerType::SOLID);
	const auto& lS = layers.find(LayerType::SOLID);

	if (lS == layers.end())
	{
		layers.at(lA).potentialEnergy += heat;
		return;
	}

	lS->second.potentialEnergy += heat * 0.5;
	layers.at(lA).potentialEnergy += heat * 0.5;
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

FlagField<TickMode> Reactor::getTickMode() const
{
	return tickMode;
}

void Reactor::setTickMode(const FlagField<TickMode> mode)
{
	tickMode.set(mode);
}

void Reactor::tick()
{
	if(tickMode.has(TickMode::ENABLE_OVERFLOW))
		checkOverflow();

	if (tickMode.has(TickMode::ENABLE_NEGLIGIBLES))
		removeNegligibles();

	if (tickMode.has(TickMode::ENABLE_REACTIONS))
	{
		findNewReactions();
		runReactions(1.0);
	}

	if (tickMode.has(TickMode::ENABLE_CONDUCTION))
		runLayerEnergyConduction(1.0);

	if (tickMode.has(TickMode::ENABLE_ENERGY))
		consumePotentialEnergy();
}

bool Reactor::hasSameState(const Reactor& other, const Amount<>::StorageType epsilon) const
{
	return this->pressure.equals(other.pressure, epsilon) &&
		this->totalMoles.equals(other.totalMoles, epsilon) &&
		this->totalMass.equals(other.totalMass, epsilon) &&
		this->totalVolume.equals(other.totalVolume, epsilon);
}

bool Reactor::hasSameContent(const Reactor& other, const Amount<>::StorageType epsilon) const
{
	return this->content.equals(other.content, epsilon);
}

bool Reactor::hasSameLayers(const Reactor& other, const Amount<>::StorageType epsilon) const
{
	auto l = LayerType::FIRST;
	while(l <= LayerType::LAST)
	{
		const bool hL = this->hasLayer(l);
		if (hL != other.hasLayer(l))
			return false;

		if (hL && (this->layers.at(l).equals(other.layers.at(l), epsilon)) == false)
			return false;
		++l;
	}

	return true;
}

bool Reactor::isSame(const Reactor& other, const Amount<>::StorageType epsilon) const
{
	return 
		this->hasSameState(other) &&
		this->hasSameContent(other) &&
		this->hasSameLayers(other);
}

Reactor Reactor::makeCopy() const
{
	return Reactor(*this);
}