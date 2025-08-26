#include "labware/LabwareConnection.hpp"

LabwareConnection::LabwareConnection(const l_size otherComponent, const uint8_t otherPort, const uint8_t strength) noexcept :
    otherComponent(otherComponent),
    otherPort(otherPort),
    strength(strength)
{}

void LabwareConnection::setFree() { otherComponent = static_cast<l_size>(-1); }

bool LabwareConnection::isFree() const { return otherComponent == static_cast<l_size>(-1); }

uint8_t LabwareConnection::getStrength(const PortType source, const PortType destination)
{
    if (source == PortType::INLET) {
        if (destination == PortType::OUTLET)
            return 250;

        if (destination == PortType::INLET)
            return 0;
    }

    if (destination == PortType::INLET) {
        if (source == PortType::OUTLET)
            return 250;

        if (source == PortType::INLET)
            return 0;
    }

    if (source == PortType::HOSE || destination == PortType::HOSE) {
        if (source != destination)
            return 0;
        return 255;
    }

    if (source == PortType::SIMPLE) {
        if (destination == PortType::SIMPLE)
            return 0;
        return 1;
    }

    if (destination == PortType::SIMPLE)
        return 220;

    if (source == PortType::DROPPER)
        return 220;

    if (destination == PortType::DROPPER)
        return 0;

    if (source == PortType::CONTACT && destination == PortType::CONTACT)
        return 255;

    return 0;
}
