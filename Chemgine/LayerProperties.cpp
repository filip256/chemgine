#include "LayerProperties.hpp"

LayerProperties::LayerProperties(Amount<Unit::CELSIUS> temperature) noexcept :
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