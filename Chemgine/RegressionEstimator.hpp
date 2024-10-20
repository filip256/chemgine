#pragma once

#include "UnitizedEstimator.hpp"
#include "EstimationMode.hpp"

template<typename RegT, Unit... InUs>
concept IsRegressor = requires(RegT reg, const Amount<InUs>... inputs) {
	{ reg.get(inputs.asStd()...) };
};


template<typename RegT, Unit OutU, Unit... InUs>
class RegressionEstimator : public UnitizedEstimator<OutU, InUs...>
{
	static_assert(IsRegressor<RegT, InUs...>,
		"RegressionEstimator: RegT must be a regressor type of the right order.");

protected:
	const RegT regressor;

public:
	using UnitizedEstimator<OutU, InUs...>::UnitizedEstimator;

public:
	RegressionEstimator(
		const EstimatorId id,
		const RegT& regressor
	) noexcept;

	EstimationMode getMode() const;
	const RegT& getRegressor() const;

	Amount<OutU> get(const Amount<InUs>... inputs) const override final;

	bool isEquivalent(const EstimatorBase& other,
		const float_n epsilon = std::numeric_limits<float_n>::epsilon()
	) const override final;

	void dumpDefinition(
		std::ostream& out,
		const bool prettify,
		std::unordered_set<EstimatorId>& alreadyPrinted,
		const bool printInline,
		const uint16_t baseIndent
	) const override final;
};

template<typename RegT, Unit OutU, Unit... InUs>
RegressionEstimator<RegT, OutU, InUs...>::RegressionEstimator(
	const EstimatorId id,
	const RegT& regressor
) noexcept :
	UnitizedEstimator<OutU, InUs...>(id),
	regressor(regressor)
{}

template<typename RegT, Unit OutU, Unit... InUs>
EstimationMode RegressionEstimator<RegT, OutU, InUs...>::getMode() const
{
	return RegT::Mode;
}

template<typename RegT, Unit OutU, Unit... InUs>
const RegT& RegressionEstimator<RegT, OutU, InUs...>::getRegressor() const
{
	return regressor;
}

template<typename RegT, Unit OutU, Unit... InUs>
Amount<OutU> RegressionEstimator<RegT, OutU, InUs...>::get(const Amount<InUs>... inputs) const
{
	return regressor.get(inputs.asStd()...);
}

template<typename RegT, Unit OutU, Unit... InUs>
bool RegressionEstimator<RegT, OutU, InUs...>::isEquivalent(const EstimatorBase& other, const float_n epsilon) const
{
	if (not UnitizedEstimator<OutU, InUs...>::isEquivalent(other, epsilon))
		return false;

	const auto& oth = static_cast<decltype(*this)&>(other);
	return this->regressor.isEquivalent(oth.regressor, epsilon);
}

template<typename RegT, Unit OutU, Unit... InUs>
void RegressionEstimator<RegT, OutU, InUs...>::dumpDefinition(
	std::ostream& out,
	const bool prettify,
	std::unordered_set<EstimatorId>& alreadyPrinted,
	const bool printInline,
	const uint16_t baseIndent) const
{
	if (not printInline && this->getRefCount() == 1)
		return;

	if (alreadyPrinted.contains(EstimatorBase::id))
	{
		if (printInline)
			out << '$' << EstimatorBase::getDefIdentifier();
		return;
	}
	alreadyPrinted.emplace(EstimatorBase::id);

	static const uint8_t valueOffset = Utils::max(
		Def::Data::Mode.size(),
		Def::Data::Parameters.size());

	DataDumper dump(out, valueOffset, baseIndent, prettify);
	if (printInline)
		dump.header("", UnitizedEstimator<OutU, InUs...>::getUnitSpecifier(), "");
	else
		dump.header(Def::Types::Data, UnitizedEstimator<OutU, InUs...>::getUnitSpecifier(), EstimatorBase::getDefIdentifier());

	dump.beginProperties()
		.propertyWithSep(Def::Data::Mode, getMode())
		.property(Def::Data::Parameters, regressor.getParams())
		.endProperties();

	if (not printInline)
		dump.endDefinition();
}
