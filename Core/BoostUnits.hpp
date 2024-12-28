#pragma once

#include <boost/units/io.hpp>
#include <boost/units/is_unit.hpp>
#include <boost/units/is_dimensionless_unit.hpp>
#include <boost/units/absolute.hpp>
#include <boost/units/base_unit.hpp>
#include <boost/units/make_system.hpp>
#include <boost/units/dimensionless_unit.hpp>
#include <boost/units/physical_dimensions.hpp>

namespace boost::units
{
	// The absolute of a unit is also a unit.
	template<class Dim, class System>
	struct boost::units::is_unit<absolute<unit<Dim, System>>> : public mpl::true_ {};
}

// Utility trait checking if a quantity in a unit can be converted to a quantity in another unit.
template<typename SrcT, typename DstT>
constexpr bool is_unit_convertible_to = !std::is_void_v<
	decltype(static_cast<boost::units::quantity<SrcT>>(std::declval<boost::units::quantity<DstT>>()))>;

// Utility trait which defines the base_unit coresponing to a given unit.
template<typename UnitT>
struct base_unit_of
{
	using type = void;
};

// Utility for storing composite unit traits.
template<typename UnitT>
struct composite_unit_trait
{
	static constexpr bool MultiplyComposite = false;
	static constexpr bool DivideComposite = false;
};

template<typename UnitT>
constexpr bool is_multiply_composite_unit = composite_unit_trait<UnitT>::MultiplyComposite;
template<typename UnitT>
constexpr bool is_divide_composite_unit = composite_unit_trait<UnitT>::DivideComposite;
template<typename UnitT>
constexpr bool is_composite_unit = is_multiply_composite_unit<UnitT> || is_divide_composite_unit<UnitT>;
template<typename UnitT>
constexpr bool is_mixed_composite_unit = is_multiply_composite_unit<UnitT> && is_divide_composite_unit<UnitT>;

// Irreducible dimensionless unit support.
namespace
{
	template <typename T, typename = void>
	struct is_dimless_convertible : std::false_type {};
	template <typename T>
	struct is_dimless_convertible<T, std::void_t<typename T::DimlessConvertible>> : T::DimlessConvertible {};
}

template <typename T>
constexpr bool is_dimless_convertible_v = is_dimless_convertible<T>::value;

template<typename DimT, long DimN>
struct ratio_base_dimension : boost::units::base_dimension<DimT, DimN> {};
template<typename DimT, long DimN>
using ratio_dimension = ratio_base_dimension<DimT, DimN>::dimension_type;

template<typename UnitT, long DimN, long UnitN>
struct ratio_base_unit : boost::units::base_unit<
	ratio_base_unit<UnitT, DimN, UnitN>,
	ratio_dimension<typename UnitT::dimension_type, DimN>,
	UnitN>
{
	using DimlessConvertible = std::true_type;

	static const char* name()
	{
		const static auto nameStr = std::string(UnitT::name()) + " / " + UnitT::name();
		return nameStr.c_str();
	}

	static const char* symbol()
	{
		const static auto symbolStr = std::string(UnitT::symbol()) + '/' + UnitT::symbol();
		return symbolStr.c_str();
	}
};

template<typename UnitT, long DimN, long UnitN>
struct percent_base_unit : boost::units::base_unit<
	percent_base_unit<UnitT, DimN, UnitN>,
	ratio_dimension<typename UnitT::dimension_type, DimN>,
	UnitN>
{
	using DimlessConvertible = std::true_type;

	static const char* name()
	{
		const static auto nameStr = std::string(UnitT::name()) + " percent";
		return nameStr.c_str();
	}

	static const char* symbol()
	{
		const static auto symbolStr = std::string(UnitT::symbol()) + '%';
		return symbolStr.c_str();
	}
};
