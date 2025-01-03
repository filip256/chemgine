#include "DynamicQuantity.hpp"
#include "Units.hpp"
#include "Log.hpp"

const UnitId DynamicQuantity::AnyUnitId = UnitId::of<AnyUnit>();

DynamicQuantity::DynamicQuantity(const float_q value, const UnitId unit) noexcept :
	val(value),
	unit(unit)
{}

float_q DynamicQuantity::value() const
{
	return val;
}

UnitId DynamicQuantity::getUnit() const
{
	return unit;
}

const DynamicQuantity::UnitInfo& DynamicQuantity::getUnitInfo(const UnitId unit)
{
	static const std::unordered_map<UnitId, UnitInfo> unitRegister
	{
		makeUnitInfo<Dimless>(),
		makeUnitInfo<Gram>(),
		makeUnitInfo<Mole>(),
		makeUnitInfo<Liter>(),
		makeUnitInfo<CubicMeter>(),
		makeUnitInfo<Drop>(),
		makeUnitInfo<Joule>(),
		makeUnitInfo<Celsius>(), makeUnitInfo<AbsCelsius>(),
		makeUnitInfo<Kelvin>(), makeUnitInfo<AbsKelvin>(),
		makeUnitInfo<Fahrenheit>(), makeUnitInfo<AbsFahrenheit>(),
		makeUnitInfo<Atmosphere>(),
		makeUnitInfo<Torr>(),
		makeUnitInfo<Pascal>(),
		makeUnitInfo<MmHg>(),
		makeUnitInfo<Second>(),
		makeUnitInfo<Meter>(),
		makeUnitInfo<Degree>(),
		makeUnitInfo<Radian>(),
		makeUnitInfo<PerMeter>(),
		makeUnitInfo<MoleRatio>(),
		makeUnitInfo<JoulePerMole>(),
		makeUnitInfo<JoulePerMoleCelsius>(),
		makeUnitInfo<MolePerSecond>(),
		makeUnitInfo<GramPerMilliLiter>(),
	};

	const auto it = unitRegister.find(unit);
	if (it == unitRegister.end())
		Log<DynamicQuantity>().fatal("Unsupported unit: '{0}'.", unit.getTypeName());

	return it->second;
}

std::optional<DynamicQuantity> DynamicQuantity::to(const UnitId target) const
{
	if (unit == target || unit == AnyUnitId || target == AnyUnitId)
		return DynamicQuantity(val, target);

	const auto& converters = getUnitInfo<Converters>(target);
	const auto it = converters.find(unit);

	return it != converters.end() ?
		std::optional((this->*(it->second))()) :
		std::nullopt;
}

const std::string& DynamicQuantity::getUnitSymbol(const UnitId unit)
{
	return getUnitInfo<Symbol>(unit);
}

const std::string& DynamicQuantity::getUnitName(const UnitId unit)
{
	return getUnitInfo<Name>(unit);
}

const std::string& DynamicQuantity::unitSymbol() const
{
	return getUnitSymbol(unit);
}

const std::string& DynamicQuantity::unitName() const
{
	return getUnitName(unit);
}

std::optional<UnitId> DynamicQuantity::parseUnitSymbol(const std::string& symbol)
{
	static const std::unordered_map<std::string, UnitId> symbolMap
	{
		{ "*", AnyUnitId },
		{ "1", UnitId::of<Dimless>() },
		{ getUnitInfo<Symbol>(UnitId::of<Gram>()), UnitId::of<Gram>() },
		{ getUnitInfo<Symbol>(UnitId::of<Mole>()), UnitId::of<Mole>() },
		{ getUnitInfo<Symbol>(UnitId::of<Liter>()), UnitId::of<Liter>() },
		{ getUnitInfo<Symbol>(UnitId::of<CubicMeter>()), UnitId::of<CubicMeter>() },
		{ getUnitInfo<Symbol>(UnitId::of<Drop>()), UnitId::of<Drop>() },
		{ getUnitInfo<Symbol>(UnitId::of<Joule>()), UnitId::of<Joule>() },
		{ getUnitInfo<Symbol>(UnitId::of<Celsius>()), UnitId::of<Celsius>() },
		{ getUnitInfo<Symbol>(UnitId::of<AbsCelsius>()), UnitId::of<AbsCelsius>() },
		{ getUnitInfo<Symbol>(UnitId::of<Kelvin>()), UnitId::of<Kelvin>() },
		{ getUnitInfo<Symbol>(UnitId::of<AbsKelvin>()), UnitId::of<AbsKelvin>() },
		{ getUnitInfo<Symbol>(UnitId::of<Fahrenheit>()), UnitId::of<Fahrenheit>() },
		{ getUnitInfo<Symbol>(UnitId::of<AbsFahrenheit>()), UnitId::of<AbsFahrenheit>() },
		{ getUnitInfo<Symbol>(UnitId::of<Atmosphere>()), UnitId::of<Atmosphere>() },
		{ getUnitInfo<Symbol>(UnitId::of<Torr>()), UnitId::of<Torr>() },
		{ getUnitInfo<Symbol>(UnitId::of<Pascal>()), UnitId::of<Pascal>() },
		{ getUnitInfo<Symbol>(UnitId::of<MmHg>()), UnitId::of<MmHg>() },
		{ getUnitInfo<Symbol>(UnitId::of<Second>()), UnitId::of<Second>() },
		{ getUnitInfo<Symbol>(UnitId::of<Meter>()), UnitId::of<Meter>() },
		{ getUnitInfo<Symbol>(UnitId::of<Degree>()), UnitId::of<Degree>() },
		{ getUnitInfo<Symbol>(UnitId::of<Radian>()), UnitId::of<Radian>() },
		{ getUnitInfo<Symbol>(UnitId::of<PerMeter>()), UnitId::of<PerMeter>() },
		{ getUnitInfo<Symbol>(UnitId::of<MoleRatio>()), UnitId::of<MoleRatio>() },
		{ getUnitInfo<Symbol>(UnitId::of<JoulePerMole>()), UnitId::of<JoulePerMole>() },
		{ getUnitInfo<Symbol>(UnitId::of<JoulePerMoleCelsius>()), UnitId::of<JoulePerMoleCelsius>() },
		{ getUnitInfo<Symbol>(UnitId::of<MolePerSecond>()), UnitId::of<MolePerSecond>() },
		{ getUnitInfo<Symbol>(UnitId::of<GramPerMilliLiter>()), UnitId::of<GramPerMilliLiter>() },
	};

	const auto it = symbolMap.find(symbol);
	return it != symbolMap.end() ?
		std::optional(it->second) :
		std::nullopt;
}

std::optional<DynamicQuantity> DynamicQuantity::parse(const float_q value, const std::string& symbol)
{
	auto unit = parseUnitSymbol(symbol);
	if (unit)
		return DynamicQuantity(value, *unit);

	unit = DynamicQuantity::parseUnitSymbol(symbol.substr(1));
	if (not unit)
		return std::nullopt;

	const uint8_t dims = unit->is<CubicMeter>() ? 3 : 1;

	switch (symbol.front())
	{
	case 'G':
		return DynamicQuantity(float_q(value * std::pow(1'000'000'000, dims)), *unit);
	case 'M':
		return DynamicQuantity(float_q(value * std::pow(1'000'000, dims)), *unit);
	case 'k':
		return DynamicQuantity(float_q(value * std::pow(1'000, dims)), *unit);
	case 'h':
		return DynamicQuantity(float_q(value * std::pow(100, dims)), *unit);
	case 'd':
		return DynamicQuantity(float_q(value * std::pow(10, -dims)), *unit);
	case 'c':
		return DynamicQuantity(float_q(value * std::pow(100, -dims)), *unit);
	case 'm':
		return DynamicQuantity(float_q(value * std::pow(1'000, -dims)), *unit);
	case 'u':
		return DynamicQuantity(float_q(value * std::pow(1'000'000, -dims)), *unit);
	case 'n':
		return DynamicQuantity(float_q(value * std::pow(1'000'000'000, -dims)), *unit);
	default:
		return std::nullopt;
	}
}

std::ostream& operator<<(std::ostream& os, const DynamicQuantity& quantity)
{
	return os << quantity.val << ' ' << quantity.unitSymbol();
}
