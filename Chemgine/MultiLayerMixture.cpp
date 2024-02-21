#include "MultiLayerMixture.hpp"
#include "Constants.hpp"

MultiLayerMixture::MultiLayerMixture(
	const Ref<Atmosphere> atmosphere,
	const Amount<Unit::LITER> maxVolume,
	const Ref<BaseContainer> overflowTarget
) noexcept :
	layers({ { LayerType::GASEOUS, LayerProperties(*this, LayerType::GASEOUS, atmosphere->getLayerProperties().temperature) }}),
	pressure(atmosphere->getPressure()),
	maxVolume(maxVolume),
	overflowTarget(overflowTarget)
{
	atmosphere->copyContentTo(*this, maxVolume);
}

bool MultiLayerMixture::tryCreateLayer(const LayerType layer)
{
	if (isRealLayer(layer) == false || layers.contains(layer))
		return false;

	const auto adjacentLayer = getClosestLayer(layer);
	layers.emplace(std::make_pair(layer, LayerProperties(*this, layer, layers.at(adjacentLayer).temperature)));
	return true;
}

void MultiLayerMixture::addToLayer(const Reactant& reactant)
{
	tryCreateLayer(reactant.layer);

	auto& layer = layers[reactant.layer];

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
		if (r->amount < Constants::MOLAR_EXISTANCE_THRESHOLD)
		{
			const auto temp = r->mutate(-r->amount);
			r = content.erase(r);
			addToLayer(temp);
			removedAny = true;
			continue;
		}

		++r;
	}

	if (removedAny == false)
		return;

	for (const auto& r : content)
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

	auto topLayer = getTopLayer();
	auto* topLayerProps = &layers.at(topLayer);

	while (overflow > topLayerProps->volume)
	{
		overflow -= topLayerProps->volume;
		moveContentTo(overflowTarget, topLayerProps->volume, topLayer);

		topLayer = getTopLayer();
		topLayerProps = &layers.at(topLayer);
	}
	
	moveContentTo(overflowTarget, overflow, topLayer);
}

LayerType MultiLayerMixture::getTopLayer() const
{
	auto l = LayerType::FIRST;
	while (l <= LayerType::LAST)
	{
		if (hasLayer(l))
			return l;
		++l;
	}
	return LayerType::NONE;
}

LayerType MultiLayerMixture::getBottomLayer() const
{
	auto l = LayerType::LAST;
	while (l > LayerType::FIRST)
	{
		if (hasLayer(l))
			return l;
		--l;
	}

	if (hasLayer(LayerType::FIRST)) // overflow protection
		return  LayerType::FIRST;

	return LayerType::NONE;
}

LayerType MultiLayerMixture::getLayerAbove(LayerType layer) const
{
	while (layer > LayerType::FIRST)
	{
		--layer;
		if (hasLayer(layer))
			return layer;
	}

	return LayerType::NONE;
}

LayerType MultiLayerMixture::getLayerBelow(LayerType layer) const
{
	while (layer < LayerType::LAST)
	{
		++layer;
		if (hasLayer(layer))
			return layer;
	}

	return LayerType::NONE;
}

LayerType MultiLayerMixture::getClosestLayer(LayerType layer) const
{
	uint8_t i = 1;
	while (true)
	{
		if (layer + i == LayerType::LAST)
		{
			return hasLayer(LayerType::LAST) ? LayerType::LAST : getLayerAbove(layer - (i - 1));
		}
		if (layer - i == LayerType::FIRST)
		{
			const auto l = getLayerBelow(layer + (i - 1));
			return l != LayerType::NONE ? l : LayerType::FIRST;
		}
		if (hasLayer(layer + i))
			return layer + i;
		if (hasLayer(layer - i))
			return layer - i;
		++i;
	}

	return LayerType::NONE;
}

void MultiLayerMixture::add(const Reactant& reactant)
{
	content.add(reactant);
	addToLayer(reactant.mutate(*this));
}

bool MultiLayerMixture::hasLayer(const LayerType layer) const
{
	const auto& l = layers.find(layer);
	return l != layers.end() && l->second.isEmpty() == false;
}

LayerType MultiLayerMixture::findLayerFor(const Reactant& reactant) const
{
	for (const auto& l : layers)
		if (reactant.getAggregation(l.second.temperature) == getAggregation(l.first))
			return l.first;

	const auto newAgg = reactant.getAggregation(layers.at(LayerType::GASEOUS).temperature);
	return getLayer(newAgg);
}

Amount<Unit::LITER> MultiLayerMixture::getMaxVolume() const
{
	return maxVolume;
}

const LayerProperties& MultiLayerMixture::getLayerProperties(const LayerType layer) const
{
	return layers.at(layer);
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
	return getLayerHeatCapacity(layer).to<Unit::JOULE_PER_MOLE>(layers.at(layer).temperature);
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

void MultiLayerMixture::copyContentTo(Ref<BaseContainer> destination, const Amount<Unit::LITER> volume, const LayerType sourceLayer) const
{
	if (destination.isSet() == false || hasLayer(sourceLayer) == false)
		return;

	// save and use initial volume
	const auto sourceVolume = layers.at(sourceLayer).volume.asStd();

	for (const auto& r : content)
	{
		if (r.layer != sourceLayer)
			continue;

		const auto molesToAdd = (r.amount / sourceVolume) * volume.asStd();
		destination->add(r.mutate(molesToAdd));
	}
}

void MultiLayerMixture::moveContentTo(Ref<BaseContainer> destination, Amount<Unit::LITER> volume, const LayerType sourceLayer)
{
	if (hasLayer(sourceLayer) == false)
		return;

	// save and use initial volume
	const auto sourceVolume = layers.at(sourceLayer).volume.asStd();

	if (volume > sourceVolume)
		volume = sourceVolume;

	const bool hasDestination = destination.isSet();
	for (const auto& r : content)
	{
		if (r.layer != sourceLayer)
			continue;

		auto molesToAdd = (r.amount / sourceVolume) * volume.asStd();
		if (hasDestination)
			destination->add(r.mutate(molesToAdd));
		add(r.mutate(-molesToAdd));
	}
}
