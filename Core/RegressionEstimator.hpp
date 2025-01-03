#pragma once

#include "UnitizedEstimator.hpp"
#include "EstimationMode.hpp"

template<typename RegT, typename... InUs>
concept IsRegressor = requires(RegT reg, const Quantity<InUs>... inputs) {
	{ reg.get(inputs.value()...) };
};


template<typename RegT, UnitType OutU, UnitType... InUs>
class RegressionEstimator : public UnitizedEstimator<OutU, InUs...>
{
	static_assert(IsRegressor<RegT, InUs...>,
		"RegressionEstimator: RegT must be a regressor type of the right order.");

protected:
	const RegT regressor;

public:
	using Base = UnitizedEstimator<OutU, InUs...>;

public:
	RegressionEstimator(
		const EstimatorId id,
		const RegT& regressor
	) noexcept;

	EstimationMode getMode() const;
	const RegT& getRegressor() const;

	Quantity<OutU> get(const Quantity<InUs>... inputs) const override final;

	bool isEquivalent(const EstimatorBase& other,
		const float_s epsilon = std::numeric_limits<float_s>::epsilon()
	) const override final;

	void dumpDefinition(
		std::ostream& out,
		const bool prettify,
		std::unordered_set<EstimatorId>& alreadyPrinted,
		const bool printInline,
		const uint16_t baseIndent
	) const override final;
};

template<typename RegT, UnitType OutU, UnitType... InUs>
RegressionEstimator<RegT, OutU, InUs...>::RegressionEstimator(
	const EstimatorId id,
	const RegT& regressor
) noexcept :
	Base(id),
	regressor(regressor)
{}

template<typename RegT, UnitType OutU, UnitType... InUs>
EstimationMode RegressionEstimator<RegT, OutU, InUs...>::getMode() const
{
	return RegT::Mode;
}

template<typename RegT, UnitType OutU, UnitType... InUs>
const RegT& RegressionEstimator<RegT, OutU, InUs...>::getRegressor() const
{
	return regressor;
}

template<typename RegT, UnitType OutU, UnitType... InUs>
Quantity<OutU> RegressionEstimator<RegT, OutU, InUs...>::get(const Quantity<InUs>... inputs) const
{
	return Quantity<OutU>::from(regressor.get(inputs.value()...));
}

template<typename RegT, UnitType OutU, UnitType... InUs>
bool RegressionEstimator<RegT, OutU, InUs...>::isEquivalent(const EstimatorBase& other, const float_s epsilon) const
{
	if (not EstimatorBase::isEquivalent(other, epsilon))
		return false;

	const auto& oth = static_cast<decltype(*this)&>(other);
	return this->regressor.isEquivalent(oth.regressor, epsilon);
}

template<typename RegT, UnitType OutU, UnitType... InUs>
void RegressionEstimator<RegT, OutU, InUs...>::dumpDefinition(
	std::ostream& out,
	const bool prettify,
	std::unordered_set<EstimatorId>& alreadyPrinted,
	const bool printInline,
	const uint16_t baseIndent) const
{
	if (not printInline && this->getRefCount() == 1)
		return;

	if (alreadyPrinted.contains(Base::id))
	{
		if (printInline)
			out << '$' << Base::getDefIdentifier();
		return;
	}
	alreadyPrinted.emplace(Base::id);

	static const auto valueOffset = checked_cast<uint8_t>(Utils::max(
		Def::Data::Mode.size(),
		Def::Data::Parameters.size()));

	Def::DataDumper dump(out, valueOffset, baseIndent, prettify);
	if (printInline)
		dump.header("", Base::getUnitSpecifier(), "");
	else
		dump.header(Def::Types::Data, Base::getUnitSpecifier(), Base::getDefIdentifier());

	dump.beginProperties()
		.propertyWithSep(Def::Data::Mode, getMode())
		.property(Def::Data::Parameters, regressor.getParams())
		.endProperties();

	if (not printInline)
		dump.endDefinition();
}
