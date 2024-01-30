#include "MultiLayerMixture.hpp"
#include "Constants.hpp"

MultiLayerMixture::MultiLayerMixture(
	const SingleLayerMixture<LayerType::GASEOUS>& atmosphere,
	const Amount<Unit::LITER> maxVolume,
	Mixture* overflowTarget
) noexcept :
	layers({ { LayerType::GASEOUS, LayerProperties(atmosphere.getLayerProperties().temperature) }}),
	pressure(atmosphere.getPressure()),
	maxVolume(maxVolume),
	overflowTarget(overflowTarget)
{
	atmosphere.copyContentTo(this, maxVolume);
}

bool MultiLayerMixture::tryCreateLayer(const LayerType layer)
{
	if (isRealLayer(layer) == false || layers.contains(layer))
		return false;

	const auto adjacentLayer = getClosestLayer(layer);
	layers.emplace(std::make_pair(layer, layers.at(getClosestLayer(layer)).temperature));
	return true;
}

void MultiLayerMixture::addToLayer(const Reactant& reactant, const uint8_t revert)
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

void MultiLayerMixture::removeFromLayer(const Reactant& reactant)
{
	addToLayer(reactant, -1);
}

void MultiLayerMixture::add(const Amount<Unit::JOULE> heat, const LayerType layer)
{
	layers.at(layer).potentialEnergy += heat;
}

void MultiLayerMixture::removeNegligibles()
{
	for (const auto& r : content)
		if (isRealLayer(r.layer) && r.amount < Constants::MOLAR_EXISTANCE_THRESHOLD)
			removeFromLayer(r);

	content.erase([](const auto& r) { return r.amount < Constants::MOLAR_EXISTANCE_THRESHOLD; });
}

void MultiLayerMixture::checkOverflow()
{
	if (maxVolume.isInfinity())
		return;

	auto overflow = totalVolume - maxVolume;
	if (overflow <= 0.0)
		return;

	auto topLayer = getTopLayer();
	auto topLayerProps = layers.at(topLayer);

	while (overflow > topLayerProps.volume)
	{
		overflow -= topLayerProps.volume;
		moveContentTo(overflowTarget, topLayerProps.volume, topLayer);

		topLayer = getTopLayer();
		topLayerProps = layers.at(topLayer);
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
	reactant.setContainer(*this);
	content.add(reactant);
	addToLayer(reactant);
}

bool MultiLayerMixture::hasLayer(const LayerType layer) const
{
	const auto& l = layers.find(layer);
	return l != layers.end() && l->second.isEmpty() == false;
}

LayerType MultiLayerMixture::findLayerFor(const Reactant& reactant) const
{
	return LayerType::POLAR;
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
	Amount<Unit::JOULE_PER_MOLE_CELSIUS> hC = 0.0;
	for (const auto& r : content)
	{
		if (r.layer == layer)
			hC += r.getHeatCapacity() * r.getMass().asStd();
	}

	return hC / layers.at(layer).mass.asStd();
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

void MultiLayerMixture::copyContentTo(Mixture* destination, const Amount<Unit::LITER> volume, const LayerType sourceLayer) const
{
	if (destination == nullptr || hasLayer(sourceLayer) == false)
		return;

	// save and use initial volume
	const auto sourceVolume = layers.at(sourceLayer).volume.asStd();

	for (const auto& r : content)
	{
		if (r.layer != sourceLayer)
			continue;

		const auto molesToAdd = (r.amount / sourceVolume) * volume.asStd();
		destination->add(Reactant(r.molecule, r.layer, molesToAdd));
	}
}

void MultiLayerMixture::moveContentTo(Mixture* destination, Amount<Unit::LITER> volume, const LayerType sourceLayer)
{
	if (hasLayer(sourceLayer) == false)
		return;

	// save and use initial volume
	const auto sourceVolume = layers.at(sourceLayer).volume.asStd();

	if (volume > sourceVolume)
		volume = sourceVolume;

	for (const auto& r : content)
	{
		if (r.layer != sourceLayer)
			continue;

		auto molesToAdd = (r.amount / sourceVolume) * volume.asStd();
		if (destination != nullptr)
			destination->add(Reactant(r.molecule, r.layer, molesToAdd));
		add(Reactant(r.molecule, r.layer, -molesToAdd));
	}
}
