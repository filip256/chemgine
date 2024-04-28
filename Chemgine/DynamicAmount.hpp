#pragma once

#include "Amount.hpp"

#include <optional>

class DynamicAmount
{
public:
	using StorageType = Amount<>::StorageType;

private:
	Unit unit;
	StorageType value = 0.0;

public:
	DynamicAmount(const Unit unit) noexcept;
	DynamicAmount(const StorageType value, const Unit unit) noexcept;
	DynamicAmount(const DynamicAmount&) = default;

	template<Unit OUnitT>
	DynamicAmount(const Amount<OUnitT> amount) noexcept;

	std::optional<DynamicAmount> to(const Unit target) const;
	template<Unit UnitT>
	std::optional<Amount<UnitT>> to() const;

	Unit getUnit() const;
	std::string getUnitSymbol() const;
	std::string getUnitName() const;

	static std::string getUnitSymbol(const Unit unit);
	static std::string getUnitName(const Unit unit);
	static std::optional<Unit> getUnitFromSymbol(const std::string& symbol);

	template<Unit UnitT>
	static inline std::optional<DynamicAmount> cast(const Amount<UnitT> amount, const Unit target) = delete;

	static std::optional<DynamicAmount> get(const StorageType value, const std::string& symbol);
	template<Unit UnitT>
	static std::optional<Amount<UnitT>> get(const StorageType value, const std::string& symbol);
};


template<Unit OUnitT>
DynamicAmount::DynamicAmount(const Amount<OUnitT> amount) noexcept :
	value(amount.asStd()),
	unit(OUnitT)
{}

template<Unit UnitT>
std::optional<Amount<UnitT>> DynamicAmount::to() const
{
	const auto converted = to(UnitT);
	return converted.has_value() ?
		std::optional(Amount<UnitT>(converted->value)) :
		std::nullopt;
}


//    --- Direct Conversions ---    //

template<>
inline std::optional<DynamicAmount> DynamicAmount::cast(const Amount<Unit::LITER> amount, const Unit target)
{
	switch (target)
	{
	case Unit::CUBIC_METER:
		return Amount<Unit::CUBIC_METER>(amount);
	case Unit::DROP:
		return Amount<Unit::DROP>(amount);
	default:
		return std::nullopt;
	}
}

template<>
inline std::optional<DynamicAmount> DynamicAmount::cast(const Amount<Unit::CUBIC_METER> amount, const Unit target)
{
	switch (target)
	{
	case Unit::LITER:
		return Amount<Unit::LITER>(amount);
	case Unit::DROP:
		return Amount<Unit::DROP>(amount);
	default:
		return std::nullopt;
	}
}

template<>
inline std::optional<DynamicAmount> DynamicAmount::cast(const Amount<Unit::DROP> amount, const Unit target)
{
	switch (target)
	{
	case Unit::LITER:
		return Amount<Unit::LITER>(amount);
	case Unit::CUBIC_METER:
		return Amount<Unit::CUBIC_METER>(amount);
	default:
		return std::nullopt;
	}
}

template<>
inline std::optional<DynamicAmount> DynamicAmount::cast(const Amount<Unit::CELSIUS> amount, const Unit target)
{
	switch (target)
	{
	case Unit::KELVIN:
		return Amount<Unit::KELVIN>(amount);
	case Unit::FAHRENHEIT:
		return Amount<Unit::FAHRENHEIT>(amount);
	default:
		return std::nullopt;
	}
}

template<>
inline std::optional<DynamicAmount> DynamicAmount::cast(const Amount<Unit::KELVIN> amount, const Unit target)
{
	switch (target)
	{
	case Unit::CELSIUS:
		return Amount<Unit::CELSIUS>(amount);
	case Unit::FAHRENHEIT:
		return Amount<Unit::FAHRENHEIT>(amount);
	default:
		return std::nullopt;
	}
}

template<>
inline std::optional<DynamicAmount> DynamicAmount::cast(const Amount<Unit::FAHRENHEIT> amount, const Unit target)
{
	switch (target)
	{
	case Unit::CELSIUS:
		return Amount<Unit::CELSIUS>(amount);
	case Unit::KELVIN:
		return Amount<Unit::KELVIN>(amount);
	default:
		return std::nullopt;
	}
}

template<>
inline std::optional<DynamicAmount> DynamicAmount::cast(const Amount<Unit::TORR> amount, const Unit target)
{
	switch (target)
	{
	case Unit::PASCAL:
		return Amount<Unit::PASCAL>(amount);
	case Unit::ATMOSPHERE:
		return Amount<Unit::ATMOSPHERE>(amount);
	default:
		return std::nullopt;
	}
}

template<>
inline std::optional<DynamicAmount> DynamicAmount::cast(const Amount<Unit::PASCAL> amount, const Unit target)
{
	switch (target)
	{
	case Unit::TORR:
		return Amount<Unit::TORR>(amount);
	case Unit::ATMOSPHERE:
		return Amount<Unit::ATMOSPHERE>(amount);
	default:
		return std::nullopt;
	}
}

template<>
inline std::optional<DynamicAmount> DynamicAmount::cast(const Amount<Unit::ATMOSPHERE> amount, const Unit target)
{
	switch (target)
	{
	case Unit::TORR:
		return Amount<Unit::TORR>(amount);
	case Unit::PASCAL:
		return Amount<Unit::PASCAL>(amount);
	default:
		return std::nullopt;
	}
}

template<>
inline std::optional<DynamicAmount> DynamicAmount::cast(const Amount<Unit::DEGREE> amount, const Unit target)
{
	switch (target)
	{
	case Unit::RADIAN:
		return Amount<Unit::RADIAN>(amount);
	default:
		return std::nullopt;
	}
}

template<>
inline std::optional<DynamicAmount> DynamicAmount::cast(const Amount<Unit::RADIAN> amount, const Unit target)
{
	switch (target)
	{
	case Unit::DEGREE:
		return Amount<Unit::DEGREE>(amount);
	default:
		return std::nullopt;
	}
}

template<Unit UnitT>
static std::optional<Amount<UnitT>> DynamicAmount::get(const StorageType value, const std::string& symbol)
{
	const auto temp = DynamicAmount::get(value, symbol);
	return temp.has_value() ?
		std::optional(temp->to<UnitT>()) :
		std::nullopt;
}
