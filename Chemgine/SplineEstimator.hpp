#pragma once

#include "UnitizedEstimator.hpp"
#include "EstimationMode.hpp"
#include "Spline.hpp"

template<Unit OutU, Unit InU>
class SplineEstimator : public UnitizedEstimator<OutU, InU>
{
private:
	const EstimationMode mode;
	const Spline<float> spline;

public:
	SplineEstimator(
		const EstimatorId id,
		Spline<float>&& spline,
		const EstimationMode mode
	) noexcept;

	SplineEstimator(
		const EstimatorId id,
		const Spline<float>& spline,
		const EstimationMode mode
	) noexcept;

	EstimationMode getMode() const;

	Amount<OutU> get(const Amount<InU> input) const override final;

	bool isEquivalent(const EstimatorBase& other,
		const float epsilon = std::numeric_limits<float>::epsilon()
	) const override final;

	void printDefinition(
		std::ostream& out,
		std::unordered_set<EstimatorId>& alreadyPrinted,
		const bool printInline
	) const override final;
};

template<Unit OutU, Unit InU>
SplineEstimator<OutU, InU>::SplineEstimator(
	const EstimatorId id,
	Spline<float>&& spline,
	const EstimationMode mode
) noexcept :
	UnitizedEstimator<OutU, InU>(id),
	mode(mode),
	spline(std::move(spline))
{}

template<Unit OutU, Unit InU>
SplineEstimator<OutU, InU>::SplineEstimator(
	const EstimatorId id,
	const Spline<float>& spline,
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
bool SplineEstimator<OutU, InU>::isEquivalent(const EstimatorBase& other, const float epsilon) const
{
	if (not EstimatorBase::isEquivalent(other, epsilon))
		return false;

	const auto& oth = static_cast<decltype(*this)&>(other);
	return this->spline.isEquivalent(oth.spline, epsilon);
}

template<Unit OutU, Unit InU>
void SplineEstimator<OutU, InU>::printDefinition(
	std::ostream& out, std::unordered_set<EstimatorId>& alreadyPrinted, const bool printInline) const
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

	out << '_';
	if (not printInline)
	{
		out << Keywords::Types::Data;
		out << '<' << EstimatorBase::getDefIdentifier() << '>';
	}
	out << ':' << UnitizedEstimator<OutU, InU>::getUnitSpecifier();

	out << '{';
	if(mode != EstimationMode::LINEAR)
		out << Keywords::Data::Mode << ':' << Def::print(mode) << ',';
	out << Keywords::Data::Values << ':' << Def::print(spline.getContent());
	out << '}';

	if (not printInline)
		out << ";\n";
}
