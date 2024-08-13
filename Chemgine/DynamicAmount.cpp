#include "DynamicAmount.hpp"
#include "Log.hpp"

#include <type_traits>
#include <unordered_map>

DynamicAmount::DynamicAmount(const Unit unit) noexcept :
	unit(unit)
{}

DynamicAmount::DynamicAmount(const StorageType value, const Unit unit) noexcept :
	value(value),
	unit(unit)
{}

DynamicAmount::StorageType DynamicAmount::asKilo() const
{
    return value / 1000.0;
}

DynamicAmount::StorageType DynamicAmount::asStd() const
{
    return value;
}

DynamicAmount::StorageType DynamicAmount::asMilli() const
{
    return value * 1000.0;
}

DynamicAmount::StorageType DynamicAmount::asMicro() const
{
    return value / 1000000.0;
}

std::optional<DynamicAmount> DynamicAmount::to(const Unit target) const
{
    if (unit == target)
        return *this;

    switch (unit)
    {
    case Unit::ANY:
        return DynamicAmount(value, target);
    case Unit::LITER:
        return DynamicAmount::cast(Amount<Unit::LITER>(value), target);
    case Unit::CUBIC_METER:
        return DynamicAmount::cast(Amount<Unit::CUBIC_METER>(value), target);
    case Unit::DROP:
        return DynamicAmount::cast(Amount<Unit::DROP>(value), target);
    case Unit::CELSIUS:
        return DynamicAmount::cast(Amount<Unit::CELSIUS>(value), target);
    case Unit::KELVIN:
        return DynamicAmount::cast(Amount<Unit::KELVIN>(value), target);
    case Unit::FAHRENHEIT:
        return DynamicAmount::cast(Amount<Unit::FAHRENHEIT>(value), target);
    case Unit::TORR:
        return DynamicAmount::cast(Amount<Unit::TORR>(value), target);
    case Unit::PASCAL:
        return DynamicAmount::cast(Amount<Unit::PASCAL>(value), target);
    case Unit::ATMOSPHERE:
        return DynamicAmount::cast(Amount<Unit::ATMOSPHERE>(value), target);
    case Unit::DEGREE:
        return DynamicAmount::cast(Amount<Unit::DEGREE>(value), target);
    case Unit::RADIAN:
        return DynamicAmount::cast(Amount<Unit::RADIAN>(value), target);
    case Unit::MOLE_RATIO:
        return DynamicAmount::cast(Amount<Unit::MOLE_RATIO>(value), target);
    case Unit::MOLE_PERCENT:
        return DynamicAmount::cast(Amount<Unit::MOLE_PERCENT>(value), target);
    default:
        return std::nullopt;
    }
}

Unit DynamicAmount::getUnit() const
{
    return unit;
}

std::string DynamicAmount::getUnitSymbol() const
{
    return DynamicAmount::getUnitSymbol(unit);
}

std::string DynamicAmount::getUnitName() const
{
    return DynamicAmount::getUnitName(unit);
}

std::string DynamicAmount::getUnitSymbol(const Unit unit)
{
    switch (unit)
    {
    case Unit::NONE:
        return Amount<Unit::NONE>::unitSymbol();
    case Unit::ANY:
        return Amount<Unit::ANY>::unitSymbol();
    case Unit::LITER:
        return Amount<Unit::LITER>::unitSymbol();
    case Unit::CUBIC_METER:
        return Amount<Unit::CUBIC_METER>::unitSymbol();
    case Unit::DROP:
        return Amount<Unit::DROP>::unitSymbol();
    case Unit::GRAM:
        return Amount<Unit::GRAM>::unitSymbol();
    case Unit::MOLE:
        return Amount<Unit::MOLE>::unitSymbol();
    case Unit::SECOND:
        return Amount<Unit::SECOND>::unitSymbol();
    case Unit::CELSIUS:
        return Amount<Unit::CELSIUS>::unitSymbol();
    case Unit::KELVIN:
        return Amount<Unit::KELVIN>::unitSymbol();
    case Unit::FAHRENHEIT:
        return Amount<Unit::FAHRENHEIT>::unitSymbol();
    case Unit::TORR:
        return Amount<Unit::TORR>::unitSymbol();
    case Unit::PASCAL:
        return Amount<Unit::PASCAL>::unitSymbol();
    case Unit::ATMOSPHERE:
        return Amount<Unit::ATMOSPHERE>::unitSymbol();
    case Unit::JOULE:
        return Amount<Unit::JOULE>::unitSymbol();
    case Unit::WATT:
        return Amount<Unit::WATT>::unitSymbol();
    case Unit::METER:
        return Amount<Unit::METER>::unitSymbol();
    case Unit::DEGREE:
        return Amount<Unit::DEGREE>::unitSymbol();
    case Unit::RADIAN:
        return Amount<Unit::RADIAN>::unitSymbol();
    case Unit::PER_SECOND:
        return Amount<Unit::PER_SECOND>::unitSymbol();
    case Unit::PER_METER:
        return Amount<Unit::PER_METER>::unitSymbol();
    case Unit::MOLE_RATIO:
        return Amount<Unit::MOLE_RATIO>::unitSymbol();
    case Unit::MOLE_PERCENT:
        return Amount<Unit::MOLE_PERCENT>::unitSymbol();
    case Unit::MOLE_PER_SECOND:
        return Amount<Unit::MOLE_PER_SECOND>::unitSymbol();
    case Unit::GRAM_PER_MOLE:
        return Amount<Unit::GRAM_PER_MOLE>::unitSymbol();
    case Unit::GRAM_PER_MILLILITER:
        return Amount<Unit::GRAM_PER_MILLILITER>::unitSymbol();
    case Unit::JOULE_PER_MOLE:
        return Amount<Unit::JOULE_PER_MOLE>::unitSymbol();
    case Unit::JOULE_PER_CELSIUS:
        return Amount<Unit::JOULE_PER_CELSIUS>::unitSymbol();
    case Unit::JOULE_PER_MOLE_CELSIUS:
        return Amount<Unit::JOULE_PER_MOLE_CELSIUS>::unitSymbol();
    case Unit::TORR_MOLE_RATIO:
        return Amount<Unit::TORR_MOLE_RATIO>::unitSymbol();
    default:
        Log<DynamicAmount>().fatal("Unable to find symbol for Unit[{0}].", static_cast<std::underlying_type_t<Unit>>(unit));
        return "";
    }
}

std::string DynamicAmount::getUnitName(const Unit unit)
{
    switch (unit)
    {
    case Unit::NONE:
        return Amount<Unit::NONE>::unitName();
    case Unit::ANY:
        return Amount<Unit::ANY>::unitName();
    case Unit::LITER:
        return Amount<Unit::LITER>::unitName();
    case Unit::CUBIC_METER:
        return Amount<Unit::CUBIC_METER>::unitName();
    case Unit::DROP:
        return Amount<Unit::DROP>::unitName();
    case Unit::GRAM:
        return Amount<Unit::GRAM>::unitName();
    case Unit::MOLE:
        return Amount<Unit::MOLE>::unitName();
    case Unit::SECOND:
        return Amount<Unit::SECOND>::unitName();
    case Unit::CELSIUS:
        return Amount<Unit::CELSIUS>::unitName();
    case Unit::KELVIN:
        return Amount<Unit::KELVIN>::unitName();
    case Unit::FAHRENHEIT:
        return Amount<Unit::FAHRENHEIT>::unitName();
    case Unit::TORR:
        return Amount<Unit::TORR>::unitName();
    case Unit::PASCAL:
        return Amount<Unit::PASCAL>::unitName();
    case Unit::ATMOSPHERE:
        return Amount<Unit::ATMOSPHERE>::unitName();
    case Unit::JOULE:
        return Amount<Unit::JOULE>::unitName();
    case Unit::WATT:
        return Amount<Unit::WATT>::unitName();
    case Unit::METER:
        return Amount<Unit::METER>::unitName();
    case Unit::DEGREE:
        return Amount<Unit::DEGREE>::unitName();
    case Unit::RADIAN:
        return Amount<Unit::RADIAN>::unitName();
    case Unit::PER_SECOND:
        return Amount<Unit::PER_SECOND>::unitName();
    case Unit::PER_METER:
        return Amount<Unit::PER_METER>::unitName();
    case Unit::MOLE_RATIO:
        return Amount<Unit::MOLE_RATIO>::unitName();
    case Unit::MOLE_PERCENT:
        return Amount<Unit::MOLE_PERCENT>::unitName();
    case Unit::MOLE_PER_SECOND:
        return Amount<Unit::MOLE_PER_SECOND>::unitName();
    case Unit::GRAM_PER_MOLE:
        return Amount<Unit::GRAM_PER_MOLE>::unitName();
    case Unit::GRAM_PER_MILLILITER:
        return Amount<Unit::GRAM_PER_MILLILITER>::unitName();
    case Unit::JOULE_PER_MOLE:
        return Amount<Unit::JOULE_PER_MOLE>::unitName();
    case Unit::JOULE_PER_CELSIUS:
        return Amount<Unit::JOULE_PER_CELSIUS>::unitName();
    case Unit::JOULE_PER_MOLE_CELSIUS:
        return Amount<Unit::JOULE_PER_MOLE_CELSIUS>::unitName();
    case Unit::TORR_MOLE_RATIO:
        return Amount<Unit::TORR_MOLE_RATIO>::unitName();
    default:
        Log<DynamicAmount>().fatal("Unable to find name for Unit[{0}].", static_cast<std::underlying_type_t<Unit>>(unit));
        return "";
    }
}

std::optional<Unit> DynamicAmount::getUnitFromSymbol(const std::string& symbol)
{
    static const std::unordered_map<std::string, Unit> symbolToUnitMap = 
    {
        {DynamicAmount::getUnitSymbol(Unit::NONE), Unit::NONE},
        {DynamicAmount::getUnitSymbol(Unit::ANY), Unit::ANY},
        {DynamicAmount::getUnitSymbol(Unit::LITER), Unit::LITER},
        {DynamicAmount::getUnitSymbol(Unit::CUBIC_METER), Unit::CUBIC_METER},
        {DynamicAmount::getUnitSymbol(Unit::DROP), Unit::DROP},
        {DynamicAmount::getUnitSymbol(Unit::GRAM), Unit::GRAM},
        {DynamicAmount::getUnitSymbol(Unit::MOLE), Unit::MOLE},
        {DynamicAmount::getUnitSymbol(Unit::SECOND), Unit::SECOND},
        {DynamicAmount::getUnitSymbol(Unit::CELSIUS), Unit::CELSIUS},
        {DynamicAmount::getUnitSymbol(Unit::KELVIN), Unit::KELVIN},
        {DynamicAmount::getUnitSymbol(Unit::FAHRENHEIT), Unit::FAHRENHEIT},
        {DynamicAmount::getUnitSymbol(Unit::TORR), Unit::TORR},
        {DynamicAmount::getUnitSymbol(Unit::PASCAL), Unit::PASCAL},
        {DynamicAmount::getUnitSymbol(Unit::ATMOSPHERE), Unit::ATMOSPHERE},
        {DynamicAmount::getUnitSymbol(Unit::JOULE), Unit::JOULE},
        {DynamicAmount::getUnitSymbol(Unit::WATT), Unit::WATT},
        {DynamicAmount::getUnitSymbol(Unit::METER), Unit::METER},
        {DynamicAmount::getUnitSymbol(Unit::DEGREE), Unit::DEGREE},
        {DynamicAmount::getUnitSymbol(Unit::RADIAN), Unit::RADIAN},
        {DynamicAmount::getUnitSymbol(Unit::PER_SECOND), Unit::PER_SECOND},
        {DynamicAmount::getUnitSymbol(Unit::PER_METER), Unit::PER_METER},
        {DynamicAmount::getUnitSymbol(Unit::MOLE_RATIO), Unit::MOLE_RATIO},
        {DynamicAmount::getUnitSymbol(Unit::MOLE_PERCENT), Unit::MOLE_PERCENT},
        {DynamicAmount::getUnitSymbol(Unit::MOLE_PER_SECOND), Unit::MOLE_PER_SECOND},
        {DynamicAmount::getUnitSymbol(Unit::GRAM_PER_MOLE), Unit::GRAM_PER_MOLE},
        {DynamicAmount::getUnitSymbol(Unit::GRAM_PER_MILLILITER), Unit::GRAM_PER_MILLILITER},
        {DynamicAmount::getUnitSymbol(Unit::JOULE_PER_MOLE), Unit::JOULE_PER_MOLE},
        {DynamicAmount::getUnitSymbol(Unit::JOULE_PER_CELSIUS), Unit::JOULE_PER_CELSIUS},
        {DynamicAmount::getUnitSymbol(Unit::JOULE_PER_MOLE_CELSIUS), Unit::JOULE_PER_MOLE_CELSIUS},
        {DynamicAmount::getUnitSymbol(Unit::TORR_MOLE_RATIO), Unit::TORR_MOLE_RATIO}
    };

    const auto it = symbolToUnitMap.find(symbol);
    return it != symbolToUnitMap.cend() ?
        std::optional(it->second) :
        std::nullopt;
}

std::optional<DynamicAmount> DynamicAmount::get(const StorageType value, const std::string& symbol)
{
    auto unit = DynamicAmount::getUnitFromSymbol(symbol);
    if (unit.has_value())
        return DynamicAmount(value, *unit);
    
    unit = DynamicAmount::getUnitFromSymbol(symbol.substr(1));
    if (unit.has_value() == false)
        return std::nullopt;
    
    const StorageType multiplier = *unit == Unit::CUBIC_METER ?
        1000000.0 :
        1000.0;
    
    switch (symbol.front())
    {
    case 'k':
        return DynamicAmount(value * multiplier, *unit);
    case 'm':
        return DynamicAmount(value / multiplier, *unit);
    case 'u':
        return DynamicAmount(value / (multiplier * multiplier), *unit);
    default:
        return std::nullopt;
    }
}
