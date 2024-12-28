#pragma once

#include "DynamicQuantity.hpp"

#include <numbers>

CHG_UNIT(Gram, "g", boost::units::mass_dimension);
CHG_UNIT(Mole, "mol", boost::units::amount_dimension);
CHG_UNIT(Liter, "L", boost::units::volume_dimension);
CHG_UNIT(Joule, "J", boost::units::energy_dimension);
CHG_UNIT(Celsius, "C", boost::units::temperature_dimension);
CHG_UNIT(Atmosphere, "atm", boost::units::pressure_dimension);
CHG_UNIT(Second, "s", boost::units::time_dimension);
CHG_UNIT(Meter, "m", boost::units::length_dimension);
CHG_UNIT(Degree, "deg", boost::units::plane_angle_dimension);

CHG_RATIO_UNIT(Gram);
CHG_RATIO_UNIT(Mole);
CHG_RATIO_UNIT(Liter);

CHG_UNIT(CubicMeter, "m^3", boost::units::volume_dimension);
CHG_UNIT_CONVERSION(CubicMeter, Liter, 1000.0, 0.0);
CHG_UNIT(Drop, "drop", boost::units::volume_dimension);
CHG_UNIT_CONVERSION(Drop, Liter, 0.00005, 0.0);
CHG_UNIT_CONVERSION(Drop, CubicMeter, 0.00000005, 0.0);
CHG_DYN_CONVERTIBLE_UNITS_3(Liter, CubicMeter, Drop);

CHG_ABSOLUTE_UNIT(Celsius);
CHG_UNIT(Kelvin, "K", boost::units::temperature_dimension);
CHG_ABSOLUTE_UNIT(Kelvin);
CHG_UNIT_CONVERSION(Kelvin, Celsius, 1.0, -273.15);
CHG_UNIT(Fahrenheit, "F", boost::units::temperature_dimension);
CHG_ABSOLUTE_UNIT(Fahrenheit);
CHG_UNIT_CONVERSION(Celsius, Fahrenheit, 1.8, 32.0);
CHG_UNIT_CONVERSION(Kelvin, Fahrenheit, 1.8, -459.67);
CHG_DYN_CONVERTIBLE_UNITS_3(Celsius, Kelvin, Fahrenheit);
CHG_DYN_CONVERTIBLE_UNITS_3(AbsCelsius, AbsKelvin, AbsFahrenheit);

CHG_UNIT(Torr, "torr", boost::units::pressure_dimension);
CHG_UNIT_CONVERSION(Torr, Atmosphere, 1.0 / 760.0, 0.0);
CHG_UNIT(Pascal, "Pa", boost::units::pressure_dimension);
CHG_UNIT_CONVERSION(Pascal, Atmosphere, 1.0 / 101325.0, 0.0);
CHG_UNIT_CONVERSION(Pascal, Torr, 760.0 / 101325.0, 0.0);
CHG_UNIT(MmHg, "mmHg", boost::units::pressure_dimension);
CHG_UNIT_CONVERSION(MmHg, Atmosphere, 1.0 / 760.0, 0.0);
CHG_UNIT_CONVERSION(MmHg, Torr, 1.0, 0.0);
CHG_UNIT_CONVERSION(MmHg, Pascal, 101325.0 / 760.0, 0.0);
CHG_DYN_CONVERTIBLE_UNITS_4(Atmosphere, Torr, Pascal, MmHg);

CHG_UNIT(Radian, "rad", boost::units::plane_angle_dimension);
CHG_UNIT_CONVERSION(Radian, Degree, 180.0 / std::numbers::pi, 0.0);

CHG_DIV_UNIT(Gram, Mole);
CHG_MUL_UNIT(Mole, Celsius);
CHG_MUL_UNIT(Mole, Fahrenheit);
CHG_DIV_UNIT(Joule, MoleCelsius);
CHG_DIV_UNIT(Joule, MoleFahrenheit);
CHG_DIV_UNIT(Joule, Second);

CHG_INV_UNIT(Second);
CHG_INV_UNIT(Meter);

CHG_SCALED_UNIT(Liter, 10, -3, Milli);
CHG_DIV_UNIT(Gram, MilliLiter);
CHG_DIV_UNIT(Gram, Liter);

CHG_UNIT(Watt, "W", boost::units::power_dimension);
CHG_UNIT_CONVERSION(Watt, JoulePerSecond, 1.0, 0.0);

CHG_UNIT_SYSTEM(ComputeUnitSystem,
	GramBaseUnit, MoleBaseUnit, LiterBaseUnit,
	JouleBaseUnit, CelsiusBaseUnit, AtmosphereBaseUnit,
	SecondBaseUnit, MeterBaseUnit, DegreeBaseUnit);
