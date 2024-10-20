#pragma once

#include "UnitizedEstimator.hpp"
#include "EstimationMode.hpp"
#include "DataDumper.hpp"
#include "Spline.hpp"

template<Unit OutU, Unit InU>
class SplineEstimator : public UnitizedEstimator<OutU, InU>
{
private:
	const EstimationMode mode;
	const Spline<float_n> spline;

public:
	SplineEstimator(
		const EstimatorId id,
		Spline<float_n>&& spline,
		const EstimationMode mode
	) noexcept;

	SplineEstimator(
		const EstimatorId id,
		const Spline<float_n>& spline,
		const EstimationMode mode
	) noexcept;

	EstimationMode getMode() const;

	Amount<OutU> get(const Amount<InU> input) const override final;

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

template<Unit OutU, Unit InU>
SplineEstimator<OutU, InU>::SplineEstimator(
	const EstimatorId id,
	Spline<float_n>&& spline,
	const EstimationMode mode
) noexcept :
	UnitizedEstimator<OutU, InU>(id),
	mode(mode),
	spline(std::move(spline))
{}

template<Unit OutU, Unit InU>
SplineEstimator<OutU, InU>::SplineEstimator(
	const EstimatorId id,
	const Spline<float_n>& spline,
	const EstimationMode mode
) noexcept :
	SplineEstimator(id, Utils::copy(spline), mode)
{}

template<Unit OutU, Unit InU>
EstimationMode SplineEstimator<OutU, InU>::getMode() const
{
	return mode;
}

template<Unit OutU, Unit InU>
Amount<OutU> SplineEstimator<OutU, InU>::get(const Amount<InU> input) const
{
	return spline.getLinearValueAt(input.asStd());
}

template<Unit OutU, Unit InU>
bool SplineEstimator<OutU, InU>::isEquivalent(const EstimatorBase& other, const float_n epsilon) const
{
	if (not EstimatorBase::isEquivalent(other, epsilon))
		return false;

	const auto& oth = static_cast<decltype(*this)&>(other);
	return this->spline.isEquivalent(oth.spline, epsilon);
}

template<Unit OutU, Unit InU>
void SplineEstimator<OutU, InU>::dumpDefinition(
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
		Def::Data::Values.size());

	DataDumper dump(out, valueOffset, baseIndent, prettify);

	if (printInline)
		dump.header("", UnitizedEstimator<OutU, InU>::getUnitSpecifier(), "");
	else
		dump.header(Def::Types::Data, UnitizedEstimator<OutU, InU>::getUnitSpecifier(), EstimatorBase::getDefIdentifier());

	dump.beginProperties()
		.propertyWithSep(Def::Data::Mode, getMode())
		.property(Def::Data::Values, spline.getContent())
		.endProperties();

	if (not printInline)
		dump.endDefinition();
}
