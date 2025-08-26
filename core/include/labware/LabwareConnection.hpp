#pragma once

#include "PortType.hpp"
#include "global/SizeTypedefs.hpp"

#include <cstdint>

class LabwareConnection
{
public:
    l_size  otherComponent;
    uint8_t otherPort;
    uint8_t strength;

    LabwareConnection(const l_size otherComponent, const uint8_t otherPort, const uint8_t strength) noexcept;

    bool isFree() const;
    void setFree();

    /// <summary>
    /// Returns the strength of the connection between two port types.
    /// The strength is a 0-255 value where 0 means the ports cannot connect,
    /// 1 means the connection is very weak and 255 means the connection is perfect.
    /// </summary>
    static uint8_t getStrength(const PortType source, const PortType destination);
};
