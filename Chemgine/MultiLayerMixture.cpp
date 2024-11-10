#include "MultiLayerMixture.hpp"
#include "Constants.hpp"

MultiLayerMixture::MultiLayerMixture(const MultiLayerMixture& other) noexcept :
	Mixture(other),
	pressure(other.pressure),
	totalMoles(other.totalMoles),
	totalMass(other.totalMass),
	totalVolume(other.totalVolume),
	maxVolume(other.maxVolume),
	overflowTarget(other.overflowTarget)
{
	for (const auto& l : other.layers)
		this->layers.emplace(std::make_pair(l.first, l.second.makeCopy(*this)));
}

MultiLayerMixture::MultiLayerMixture(
	const Ref<Atmosphere> atmosphere,
	const Amount<Unit::LITER> maxVolume,
	const Ref<ContainerBase> overflowTarget
) noexcept :
	pressure(atmosphere->getPressure()),
	maxVolume(maxVolume),
	overflowTarget(overflowTarget)
{
	layers.emplace(LayerType::GASEOUS, Layer(*this, LayerType::GASEOUS, atmosphere->getLayer().temperature));
	atmosphere->copyContentTo(*this, maxVolume);
}

bool MultiLayerMixture::tryCreateLayer(const LayerType layer)
{
	if (isRealLayer(layer) == false || layers.contains(layer))
		return false;

	const auto adjacentLayer = getClosestLayer(layer);
	layers.emplace(std::make_pair(layer, Layer(*this, layer, layers.at(adjacentLayer).temperature)));
	return true;
}

void MultiLayerMixture::addToLayer(const Reactant& reactant)
{
	tryCreateLayer(reactant.layer);

	auto& layer = layers[reactant.layer];

	// add polarity to layer average polarity
	const auto polarity = reactant.molecule.getPolarity();
	layer.polarity.hydrophilicity =
		((layer.polarity.hydrophilicity * layer.moles.asStd()) + polarity.hydrophilicity) /
		(layer.moles + reactant.amount).asStd();
	layer.polarity.lipophilicity =
		((layer.polarity.lipophilicity * layer.moles.asStd()) + polarity.lipophilicity) /
		(layer.moles + reactant.amount).asStd();

	totalMoles += reactant.amount;
	layer.moles += reactant.amount;

	const auto mass = reactant.getMass();
	totalMass += mass;
	layer.mass += mass;

	const auto vol = reactant.getVolume();
	totalVolume += vol;
	layer.volume += vol;

	if (reactant.isNew)
		layer.setIfNucleator(reactant);
	else if (content.contains(reactant) == false)
		layer.unsetIfNucleator(reactant);
}

void MultiLayerMixture::add(const Amount<Unit::JOULE> heat, const LayerType layer)
{
	layers.at(layer).potentialEnergy += heat;
}

void MultiLayerMixture::removeNegligibles()
{
	bool removedAny = false;
	for (auto r = content.begin(); r != content.end();)
	{
		if (r->second.amount < Constants::MOLAR_EXISTANCE_THRESHOLD)
		{
			const auto temp = r->second.mutate(-r->second.amount);
			r = content.erase(r);
			addToLayer(temp);
			removedAny = true;
			continue;
		}

		++r;
	}

	if (removedAny == false)
		return;

	for (const auto& [_, r] : content)
	{
		layers.at(r.layer).setIfNucleator(r);
	}
}

void MultiLayerMixture::checkOverflow()
{
	if (maxVolume.isInfinity())
		return;

	auto overflow = totalVolume - maxVolume;
	if (overflow <= 0.0)
		return;

	auto topLayerType = getTopLayer();
	auto* topLayer = &layers.at(topLayerType);

	while (overflow > topLayer->volume)
	{
		overflow -= topLayer->volume;
		moveContentTo(overflowTarget, topLayer->volume, topLayerType);

		topLayerType = getTopLayer();
		topLayer = &layers.at(topLayerType);
	}
	
	moveContentTo(overflowTarget, overflow, topLayerType);
}

LayerType MultiLayerMixture::getTopLayer() const
{
	for (auto l = layers.cbegin(); l != layers.cend(); ++l)
		if (l->second.isEmpty() == false)
			return l->first;

	return LayerType::NONE;
}

LayerType MultiLayerMixture::getBottomLayer() const
{
	for (auto l = layers.crbegin(); l != layers.crend(); ++l)
		if (l->second.isEmpty() == false)
			return l->first;

	return LayerType::NONE;
}

LayerType MultiLayerMixture::getLayerAbove(LayerType layer) const
{
	auto l = std::map<LayerType, Layer>::const_reverse_iterator(layers.lower_bound(layer));
	if (l != layers.crend() && l->first == layer)
		++l;

	for (; l != layers.crend(); ++l)
		if (l->second.isEmpty() == false)
			return l->first;

	return LayerType::NONE;
}

LayerType MultiLayerMixture::getLayerBelow(LayerType layer) const
{
	for (auto l = layers.upper_bound(layer); l != layers.cend(); ++l)
		if (l->second.isEmpty() == false)
			return l->first;

	return LayerType::NONE;
}

LayerType MultiLayerMixture::getClosestLayer(LayerType layer) const
{
	if (layers.empty())
		return LayerType::NONE;

	auto l = layers.lower_bound(layer);	
	auto below = (l != layers.cend() && l->first == layer) ? std::next(l) : l;
	auto above = std::map<LayerType, Layer>::const_reverse_iterator(l);

	while (below != layers.cend() && above != layers.crend())
	{
		if (below->second.isEmpty() == false)
			return below->first;
		if (above->second.isEmpty() == false)
			return above->first;

		++below;
		++above;
	}

	while (below != layers.cend())
	{
		if (below->second.isEmpty() == false)
			return below->first;
		++below;
	}

	while (above != layers.crend())
	{
		if (above->second.isEmpty() == false)
			return above->first;
		++above;
	}

	return LayerType::NONE;
}

bool MultiLayerMixture::areAdjacentLayers(LayerType layer1, LayerType layer2) const
{
	if (layer1 > layer2)
		std::swap(layer1, layer2);

	if (toIndex(layer2) - toIndex(layer1) <= 1)
		return true;

	return getLayerBelow(layer2) == layer1;
}

LayerType MultiLayerMixture::findLayerFor(const Reactant& reactant) const
{
	for (const auto& l : layers)
	{
		const auto rAggr = reactant.getAggregationAt(l.second.temperature);
		const auto lAggr = getAggregationType(l.first);

		if (rAggr != lAggr)
			continue;

		if (lAggr != AggregationType::LIQUID)
			return l.first;

		const auto polarity = reactant.molecule.getPolarity();
		const auto density = reactant.molecule.getDensityAt(l.second.temperature, pressure);
		return getLayerType(getAggregationType(l.first), polarity.getPartitionCoefficient() > 1.0, density > 1.0);
	}

	const auto defaultT = layers.at(LayerType::GASEOUS).temperature;
	const auto newAgg = reactant.getAggregationAt(defaultT);
	const auto polarity = reactant.molecule.getPolarity();
	const auto density = reactant.molecule.getDensityAt(defaultT, pressure);
	return getLayerType(newAgg, polarity.getPartitionCoefficient() > 1.0, density > 1.0);
}

void MultiLayerMixture::add(const Reactant& reactant)
{
	content.add(reactant);
	addToLayer(reactant.mutate(*this));
}

void MultiLayerMixture::add(const Molecule& molecule, const Amount<Unit::MOLE> amount)
{
	const auto r = Reactant(molecule, LayerType::NONE, amount, *this);
	add(r.mutate(findLayerFor(r)));
}

void MultiLayerMixture::add(const Amount<Unit::JOULE> heat)
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

bool MultiLayerMixture::hasLayer(const LayerType layer) const
{
	const auto& l = layers.find(layer);
	return l != layers.cend() && l->second.isEmpty() == false;
}

Amount<Unit::LITER> MultiLayerMixture::getMaxVolume() const
{
	return maxVolume;
}

const Layer& MultiLayerMixture::getLayer(const LayerType layer) const
{
	return layers.at(layer);
}

Amount<Unit::CELSIUS> MultiLayerMixture::getLayerTemperature(const LayerType layer) const
{
	return layers.at(layer).temperature;
}

Amount<Unit::JOULE_PER_MOLE_CELSIUS> MultiLayerMixture::getLayerHeatCapacity(const LayerType layer) const
{
	return layers.at(layer).getHeatCapacity();
}

Amount<Unit::JOULE_PER_CELSIUS> MultiLayerMixture::getLayerTotalHeatCapacity(const LayerType layer) const
{
	return layers.at(layer).getTotalHeatCapacity();
}

Amount<Unit::JOULE_PER_MOLE> MultiLayerMixture::getLayerKineticEnergy(const LayerType layer) const
{
	return layers.at(layer).getKineticEnergy();
}

Polarity MultiLayerMixture::getLayerPolarity(const LayerType layer) const
{
	return layers.at(layer).getPolarity();
}

Color MultiLayerMixture::getLayerColor(const LayerType layer) const
{
	return layers.at(layer).getColor();
}

bool MultiLayerMixture::isEmpty() const
{
	return totalMoles == 0.0_mol;
}

Ref<ContainerBase> MultiLayerMixture::getOverflowTarget() const
{
	return overflowTarget;
}

void MultiLayerMixture::setOverflowTarget(const Ref<ContainerBase> target)
{
	if (this == &*target)
		Log(this).warn("Overflow target set to self.");

	overflowTarget = target;
}

Amount<Unit::TORR> MultiLayerMixture::getPressure() const
{
	return pressure;
}

Amount<Unit::MOLE> MultiLayerMixture::getTotalMoles() const
{
	return totalMoles;
}

Amount<Unit::GRAM> MultiLayerMixture::getTotalMass() const
{
	return totalMass;
}

Amount<Unit::LITER> MultiLayerMixture::getTotalVolume() const
{
	return totalVolume;
}

MultiLayerMixture::LayerDownIterator MultiLayerMixture::getLayersDownBegin() const
{
	return layers.cbegin();
}

MultiLayerMixture::LayerDownIterator MultiLayerMixture::getLayersDownEnd() const
{
	return layers.cend();
}

MultiLayerMixture::LayerUpIterator MultiLayerMixture::getLayersUpBegin() const
{
	return layers.crbegin();
}

MultiLayerMixture::LayerUpIterator MultiLayerMixture::getLayersUpEnd() const
{
	return layers.crend();
}

void MultiLayerMixture::copyContentTo(Ref<ContainerBase> destination, const Amount<Unit::LITER> volume, const LayerType sourceLayer) const
{
	if (hasLayer(sourceLayer) == false)
		return;

	// save and use initial volume
	const auto sourceVolume = layers.at(sourceLayer).volume.asStd();

	for (const auto& [_, r] : content)
	{
		if (r.layer != sourceLayer)
			continue;

		const auto molesToAdd = (r.amount / sourceVolume) * volume.asStd();
		destination->add(r.mutate(molesToAdd));
	}
}

void MultiLayerMixture::moveContentTo(Ref<ContainerBase> destination, Amount<Unit::LITER> volume, const LayerType sourceLayer)
{
	if (hasLayer(sourceLayer) == false)
		return;

	// save and use initial volume
	const auto sourceVolume = layers.at(sourceLayer).volume.asStd();

	if (volume > sourceVolume)
		volume = sourceVolume;

	for (auto& [_, r] : content)
	{
		if (r.layer != sourceLayer)
			continue;

		auto molesToAdd = (r.amount / sourceVolume) * volume.asStd();
		destination->add(r.mutate(molesToAdd));
		add(r.mutate(-molesToAdd));
	}
}

MultiLayerMixture MultiLayerMixture::makeCopy() const
{
	return MultiLayerMixture(*this);
}
