#pragma once

#include "estimators/kinds/UnitizedEstimator.hpp"
#include "estimators/EstimationMode.hpp"
#include "data/def/DataDumper.hpp"
#include "structs/Spline.hpp"

template<Unit OutU, Unit InU>
class SplineEstimator : public UnitizedEstimator<OutU, InU>
{
private:
	using Base = UnitizedEstimator<OutU, InU>;

	const EstimationMode mode;
	const Spline<float_s> spline;

public:
	SplineEstimator(
		const EstimatorId id,
		Spline<float_s>&& spline,
		const EstimationMode mode
	) noexcept;

	SplineEstimator(
		const EstimatorId id,
		const Spline<float_s>& spline,
		const EstimationMode mode
	) noexcept;

	EstimationMode getMode() const;

	Amount<OutU> get(const Amount<InU> input) const override final;

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

template<Unit OutU, Unit InU>
SplineEstimator<OutU, InU>::SplineEstimator(
	const EstimatorId id,
	Spline<float_s>&& spline,
	const EstimationMode mode
) noexcept :
	UnitizedEstimator<OutU, InU>(id),
	mode(mode),
	spline(std::move(spline))
{}

template<Unit OutU, Unit InU>
SplineEstimator<OutU, InU>::SplineEstimator(
	const EstimatorId id,
	const Spline<float_s>& spline,
	const EstimationMode mode
) noexcept :
	SplineEstimator(id, utils::copy(spline), mode)
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
bool SplineEstimator<OutU, InU>::isEquivalent(const EstimatorBase& other, const float_s epsilon) const
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

	if (alreadyPrinted.contains(Base::id))
	{
		if (printInline)
			out << '$' << Base::getDefIdentifier();
		return;
	}
	alreadyPrinted.emplace(Base::id);

	static const auto valueOffset = checked_cast<uint8_t>(utils::max(
		def::Data::Mode.size(),
		def::Data::Values.size()));

	def::DataDumper dump(out, valueOffset, baseIndent, prettify);
	if (printInline)
		dump.header("", Base::getUnitSpecifier(), "");
	else
		dump.header(def::Types::Data, Base::getUnitSpecifier(), Base::getDefIdentifier());

	dump.beginProperties()
		.propertyWithSep(def::Data::Mode, getMode());

	if (prettify)
	{
		const auto& content = spline.getContent();
		std::vector<DataPoint<OutU, InU>> values;
		values.reserve(content.size());
		std::transform(content.begin(), content.end(), std::back_inserter(values),
			[](const auto& v) { return DataPoint<OutU, InU>(v.second, v.first); });

		dump.property(def::Data::Values, values);
	}
	else
		dump.property(def::Data::Values, spline.getContent());
	
	dump.endProperties();

	if (not printInline)
		dump.endDefinition();
}
