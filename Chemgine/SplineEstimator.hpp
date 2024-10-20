#pragma once

#include "UnitizedEstimator.hpp"
#include "EstimationMode.hpp"
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

	out << '_';
	if (not printInline)
	{
		out << Def::Types::Data;
		out << '<' << EstimatorBase::getDefIdentifier() << '>';
	}

	if (prettify)
	{
		static const uint8_t valueOffset = Utils::max(
			Def::Data::Mode.size(),
			Def::Data::Values.size()) + 2;

		out << ": " << UnitizedEstimator<OutU, InU>::getUnitSpecifier(true);
		out << " {\n";

		std::string indentStr(baseIndent, ' ');
		out << indentStr << Def::Syntax::Indent << Def::Data::Mode << ':' << std::string(valueOffset - Def::Data::Mode.size(), ' ')
			<< Def::prettyPrint(getMode()) << ",\n";
		out << indentStr << Def::Syntax::Indent << Def::Data::Values << ':' << std::string(valueOffset - Def::Data::Values.size(), ' ')
			<< Def::prettyPrint(spline.getContent());
		out << '\n' << indentStr << '}';

		if (not printInline)
			out << indentStr << ";\n\n";
	}
	else
	{
		out << ':' << UnitizedEstimator<OutU, InU>::getUnitSpecifier(false);

		out << '{';
		if (mode != EstimationMode::LINEAR)
			out << Def::Data::Mode << ':' << Def::print(mode) << ',';
		out << Def::Data::Values << ':' << Def::print(spline.getContent());
		out << '}';

		if (not printInline)
			out << ";\n";
	}
}
