#include "LayerProperties.hpp"

LayerProperties::LayerProperties(const Amount<Unit::CELSIUS> temperature) noexcept :
	temperature(temperature)
{}

Amount<Unit::CELSIUS> LayerProperties::getTemperature() const {
    return temperature;
}

Amount<Unit::MOLE> LayerProperties::getMoles() const {
    return moles;
}

Amount<Unit::GRAM> LayerProperties::getMass() const {
    return mass;
}

Amount<Unit::LITER> LayerProperties::getVolume() const {
    return volume;
}

bool LayerProperties::isEmpty() const
{
    return moles == 0.0;
}

bool LayerProperties::operator==(const LayerProperties& other) const
{
    return this->temperature == other.temperature &&
        this->moles == other.moles &&
        this->mass == other.mass &&
        this->volume == other.volume &&
        this->potentialEnergy == other.potentialEnergy;
}

bool LayerProperties::operator!=(const LayerProperties& other) const
{
    return !(*this == other);
}
