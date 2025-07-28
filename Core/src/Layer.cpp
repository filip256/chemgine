#include "Layer.hpp"
#include "Mixture.hpp"
#include "Constants.hpp"

Layer::Layer(
    const Ref<Mixture> container,
    const LayerType layerType,
    const Amount<Unit::CELSIUS> temperature
) noexcept :
    container(container),
    layerType(layerType),
	temperature(temperature)
{
    if (isLiquidLayer(layerType))
    {
        lowNucleator.setTransitionPointCB(&Reactant::getMeltingPoint);
        lowNucleator.setTransitionHeatCB(&Reactant::getFusionHeat);
        highNucleator.setTransitionPointCB(&Reactant::getBoilingPoint);
        highNucleator.setTransitionHeatCB(&Reactant::getVaporizationHeat);
    }
    else if (isGasLayer(layerType))
    {
        lowNucleator.setTransitionPointCB(&Reactant::getBoilingPoint);
        lowNucleator.setTransitionHeatCB(&Reactant::getCondensationHeat);
    }
    else if (isSolidLayer(layerType))
    {
        highNucleator.setTransitionPointCB(&Reactant::getMeltingPoint);
        highNucleator.setTransitionHeatCB(&Reactant::getLiquefactionHeat);
    }
}

void Layer::findNewLowNucleator()
{
    lowNucleator.unset();
    for (const auto& [_, r] : container->content)
        if (r.layer == layerType && r.amount >= Constants::MOLAR_EXISTANCE_THRESHOLD)
            lowNucleator.setIfLower(r);
}

void Layer::findNewHighNucleator()
{
    highNucleator.unset();
    for (const auto& [_, r] : container->content)
        if (r.layer == layerType && r.amount >= Constants::MOLAR_EXISTANCE_THRESHOLD)
            highNucleator.setIfLower(r);
}

void Layer::consumePositivePotentialEnergy()
{
    const auto higherAggregationLayer = getHigherAggregationLayer(layerType);
    while (isEmpty() == false)
    {
        const auto hC = getTotalHeatCapacity();

        // no transition point
        if (highNucleator.isNull())
        {
            temperature += potentialEnergy.to<Unit::CELSIUS>(hC);
            potentialEnergy = 0.0_J;
            return;
        }

        // reach transition point
        const auto maxT = getMaxAllowedTemperature();
        auto reqE = hC.to<Unit::JOULE>(maxT - temperature);
        if (reqE >= potentialEnergy)
        {
            temperature += potentialEnergy.to<Unit::CELSIUS>(hC);
            potentialEnergy = 0.0_J;
            return;
        }
        temperature = maxT;
        potentialEnergy -= reqE;

        // change the state of the nucleator
        const auto& nucleator = highNucleator.getReactant();
        const auto maxMoles = container->getAmountOf(nucleator);
        const auto lH = highNucleator.getTransitionHeat();
        const auto convMoles = potentialEnergy.to<Unit::MOLE>(lH);
        if (maxMoles >= convMoles)
        {
            container->add(nucleator.mutate(convMoles, higherAggregationLayer));
            container->add(nucleator.mutate(-convMoles));
            potentialEnergy = 0.0_J;
            return;
        }
        container->add(nucleator.mutate(maxMoles, higherAggregationLayer));
        container->add(nucleator.mutate(-maxMoles));
        potentialEnergy -= lH.to<Unit::JOULE>(maxMoles);

        // find the new nucleator, repeat
        findNewHighNucleator();
    }

    if(higherAggregationLayer != LayerType::NONE)
        container->add(potentialEnergy, higherAggregationLayer);

    potentialEnergy = 0.0_J;
    temperature = Amount<Unit::CELSIUS>::Infinity;
}

void Layer::consumeNegativePotentialEnergy()
{
    const auto lowerAggregationLayer = getLowerAggregationLayer(layerType);
    while (isEmpty() == false)
    {
        const auto hC = getTotalHeatCapacity();

        // no transition point
        if (lowNucleator.isNull())
        {
            temperature += potentialEnergy.to<Unit::CELSIUS>(hC);
            potentialEnergy = 0.0_J;
            return;
        }

        // reach transition point
        const auto minT = getMinAllowedTemperature();
        auto reqE = hC.to<Unit::JOULE>(minT - temperature);
        if (reqE <= potentialEnergy)
        {
            temperature += potentialEnergy.to<Unit::CELSIUS>(hC);
            potentialEnergy = 0.0_J;
            return;
        }
        temperature = minT;
        potentialEnergy -= reqE;

        // change the state of the whole nucleator
        const auto& nucleator = lowNucleator.getReactant();
        const auto maxMoles = container->getAmountOf(nucleator);
        const auto lH = lowNucleator.getTransitionHeat();
        const auto convMoles = potentialEnergy.to<Unit::MOLE>(lH);
        if (maxMoles >= convMoles)
        {
            container->add(nucleator.mutate(convMoles, lowerAggregationLayer));
            container->add(nucleator.mutate(-convMoles));
            potentialEnergy = 0.0_J;
            return;
        }
        container->add(nucleator.mutate(maxMoles, lowerAggregationLayer));
        container->add(nucleator.mutate(-maxMoles));
        potentialEnergy -= lH.to<Unit::JOULE>(maxMoles);

        // find the new nucleator, repeat
        findNewLowNucleator();
    }

    if (lowerAggregationLayer != LayerType::NONE)
        container->add(potentialEnergy, lowerAggregationLayer);

    potentialEnergy = 0.0_J;
    temperature = Amount<Unit::CELSIUS>::Infinity;
}

Amount<Unit::JOULE> Layer::getLeastEnergyDiff(const Amount<Unit::CELSIUS> target) const
{
    if (target > temperature)
    {
        const auto transitionPoint = getMaxAllowedTemperature();
        if (temperature == transitionPoint && transitionPoint != Amount<Unit::CELSIUS>::Maximum)
        {
            const auto nMoles = container->content.getAmountOf(highNucleator.getReactant());
            return highNucleator.getTransitionHeat().to<Unit::JOULE>(nMoles);
        }
        
        return getTotalHeatCapacity()
            .to<Unit::JOULE>(std::min(target, transitionPoint) - temperature);
    }
    
    const auto transitionPoint = getMinAllowedTemperature();
    if (temperature == transitionPoint && transitionPoint != Amount<Unit::CELSIUS>::Minimum)
    {
        const auto nMoles = container->content.getAmountOf(lowNucleator.getReactant());
        return lowNucleator.getTransitionHeat().to<Unit::JOULE>(nMoles);
    }

    return getTotalHeatCapacity()
        .to<Unit::JOULE>(std::max(target, transitionPoint) - temperature);
}

bool Layer::hasTemporaryState(const Reactant& reactant) const
{
    if (isLiquidLayer(layerType))
        return reactant.getBoilingPoint() < temperature || reactant.getMeltingPoint() > temperature;
    if (isGasLayer(layerType))
        return reactant.getBoilingPoint() > temperature;

    return reactant.getMeltingPoint() < temperature;
}

void Layer::convertTemporaryStateReactants()
{
    // TODO: handle equal distribution with multiple temp state reactants
    //        - temp reactats with smaller diffs act towards reaching the tp of those with higher or equal diffs
    //        - each contributes proportionally to its diff * mass
    if (isLiquidLayer(layerType))
    {
        for (const auto& [_, r] : container->content)
        {
            if (r.layer != layerType)
                continue;

            auto tp = r.getBoilingPoint();
            if (tp < temperature)
            {
                const auto lH = r.getVaporizationHeat();
                const auto convMoles = std::min(
                    r.amount, lH.to<Unit::MOLE>(getLeastEnergyDiff(tp)));
                container->add(r.mutate(convMoles, LayerType::GASEOUS));
                container->add(r.mutate(-convMoles));
                container->add(lH.to<Unit::JOULE>(convMoles), layerType);
                continue;
            }

            tp = r.getMeltingPoint();
            if (tp > temperature)
            {
                const auto lH = r.getFusionHeat();
                const auto convMoles = std::min(
                    r.amount, lH.to<Unit::MOLE>(getLeastEnergyDiff(tp)));
                container->add(r.mutate(convMoles, LayerType::SOLID));
                container->add(r.mutate(-convMoles));
                container->add(lH.to<Unit::JOULE>(convMoles), layerType);
            }
        }
    }
    else if (isGasLayer(layerType))
    {
        for (const auto& [_, r] : container->content)
        {
            if (r.layer != layerType)
                continue;

            const auto ltp = r.getBoilingPoint();
            if (ltp > temperature)
            {
                const auto lH = r.getCondensationHeat();
                const auto convMoles = std::min(
                    r.amount, lH.to<Unit::MOLE>(getLeastEnergyDiff(ltp)));
                container->add(r.mutate(convMoles, LayerType::POLAR));
                container->add(r.mutate(-convMoles));
                container->add(lH.to<Unit::JOULE>(convMoles), layerType);
            }
        }
    }
    else if (isSolidLayer(layerType))
    {
        for (const auto& [_, r] : container->content)
        {
            if (r.layer != layerType)
                continue;

            const auto htp = r.getMeltingPoint();
            if (htp < temperature)
            {
                const auto lH = r.getLiquefactionHeat();
                const auto convMoles = std::min(
                    r.amount, lH.to<Unit::MOLE>(getLeastEnergyDiff(htp)));
                container->add(r.mutate(convMoles, LayerType::POLAR));
                container->add(r.mutate(-convMoles));
                container->add(lH.to<Unit::JOULE>(convMoles), layerType);
            }
        }
    }
}

LayerType Layer::getType() const
{
    return layerType;
}

Amount<Unit::MOLE> Layer::getMoles() const 
{
    return moles;
}

Amount<Unit::GRAM> Layer::getMass() const 
{
    return mass;
}

Amount<Unit::LITER> Layer::getVolume() const 
{
    return volume;
}

bool Layer::hasLowNucleator() const
{
    return lowNucleator.isValid();
}

bool Layer::hasHighNucleator() const
{
    return highNucleator.isValid();
}

const Reactant& Layer::getLowNucleator() const
{
    return lowNucleator.getReactant();
}

const Reactant& Layer::getHighNucleator() const
{
    return highNucleator.getReactant();
}

Amount<Unit::CELSIUS> Layer::getTemperature() const 
{
    return temperature;
}

Amount<Unit::CELSIUS> Layer::getMinAllowedTemperature() const
{
    return lowNucleator.isValid() ?
        lowNucleator.getTransitionPoint() :
        Amount<Unit::CELSIUS>::Minimum;
}

Amount<Unit::CELSIUS> Layer::getMaxAllowedTemperature() const
{
    return highNucleator.isValid() ?
        highNucleator.getTransitionPoint() :
        Amount<Unit::CELSIUS>::Maximum;
}

Amount<Unit::JOULE_PER_MOLE_CELSIUS> Layer::getHeatCapacity() const
{
    Amount<Unit::JOULE_PER_MOLE_CELSIUS> hC = 0.0;
    Amount<Unit::GRAM> ms = 0.0_g;
    for (const auto& [_, r] : container->content)
    {
        if (r.layer == layerType && hasTemporaryState(r) == false)
        {
            hC += r.getHeatCapacity() * r.getMass().asStd();
            ms += r.getMass();  // ignore temp state reactants
        }
    }

    return hC / ms.asStd();
}

Amount<Unit::JOULE_PER_CELSIUS> Layer::getTotalHeatCapacity() const
{
    Amount<Unit::JOULE_PER_MOLE_CELSIUS> hC = 0.0;
    Amount<Unit::GRAM> ms = 0.0_g;
    Amount<Unit::MOLE> mo = 0.0_mol;
    for (const auto& [_, r] : container->content)
    {
        if (r.layer == layerType && hasTemporaryState(r) == false)
        {
            hC += r.getHeatCapacity() * r.getMass().asStd();
            ms += r.getMass();  // ignore temp state reactants
            mo += r.amount;
        }
    }

    return (hC / ms.asStd()).to<Unit::JOULE_PER_CELSIUS>(mo);
}

Amount<Unit::JOULE_PER_MOLE> Layer::getKineticEnergy() const
{
    return getHeatCapacity().to<Unit::JOULE_PER_MOLE>(temperature);
}

Polarity Layer::getPolarity() const
{
    return polarity;
}

Color Layer::getColor() const
{
    if (isEmpty())
        return Color();

    float_s div = 0.0;
    float_s red = 0.0, green = 0.0, blue = 0.0, alpha = 0.0;
    for (const auto& [_, r] : container->content)
    {
        if (r.layer == layerType)
        {
            const auto color = r.molecule.getColor();
            const auto amount = r.amount.asStd();
            red += color.r * color.a * amount;
            green += color.g * color.a * amount;
            blue += color.b * color.a * amount;
            alpha += color.a * color.a * amount;
            div += color.a * amount;
        }
    }

    alpha /= div;
    alpha =
        isGasLayer(layerType) ? (alpha * 50) / 255 :
        isLiquidLayer(layerType) ? (alpha * 150) / 255 :
        alpha;

    return Color(
        static_cast<uint8_t>(red / div),
        static_cast<uint8_t>(green / div),
        static_cast<uint8_t>(blue / div),
        static_cast<uint8_t>(alpha));
}

bool Layer::isEmpty() const
{
    return moles == 0.0;
}

void Layer::setIfNucleator(const Reactant& reactant)
{
    lowNucleator.setIfLower(reactant);
    highNucleator.setIfHigher(reactant);
}

void Layer::unsetIfNucleator(const Reactant& reactant)
{
    if (lowNucleator.isSet() && lowNucleator.getReactant().molecule == reactant.molecule)
        lowNucleator.unset();
    if (highNucleator.isSet() && highNucleator.getReactant().molecule == reactant.molecule)
        highNucleator.unset();
}

void Layer::consumePotentialEnergy()
{
    if (potentialEnergy > 0.0)
        consumePositivePotentialEnergy();
    else if(potentialEnergy < 0.0)
        consumeNegativePotentialEnergy();
}

bool Layer::equals(const Layer& other, const Amount<>::StorageType epsilon) const
{
    return this->temperature.equals(other.temperature, epsilon) &&
        this->potentialEnergy.equals(other.potentialEnergy, epsilon) &&
        this->moles.equals(other.moles, epsilon) &&
        this->mass.equals(other.mass, epsilon) &&
        this->volume.equals(other.volume, epsilon);
}

LayerContentIterator Layer::begin() const
{
    return LayerContentIterator(layerType, container->content.begin(), container->content.end());
}

LayerContentIterator Layer::end() const
{
    return LayerContentIterator(layerType, container->content.end(), container->content.end());
}

Layer Layer::makeCopy(const Ref<Mixture> newContainer) const
{
    auto temp = Layer(*this);
    temp.container = newContainer;
    if (temp.lowNucleator.isSet())
        temp.lowNucleator.setReactant(temp.lowNucleator.getReactant().mutate(newContainer));
    if (temp.highNucleator.isSet())
        temp.highNucleator.setReactant(temp.highNucleator.getReactant().mutate(newContainer));
    return temp;
}
