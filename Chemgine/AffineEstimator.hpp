#pragma once

#include "DerivedEstimator.hpp"
#include "Keywords.hpp"
#include "Printers.hpp"

template<Unit OutU, Unit InU>
class AffineEstimator : public DerivedEstimator<OutU, InU>
{
private:
	const EstimatorRef<OutU, InU> base;

	void tryPrintOOLDefinition(
		std::ostream& out,
		const bool prettify,
		std::unordered_set<EstimatorId>& alreadyPrinted,
		const uint16_t baseIndent
	) const override final;

	void printILDefinition(
		std::ostream& out,
		const bool prettify,
		std::unordered_set<EstimatorId>& alreadyPrinted,
		const uint16_t baseIndent
	) const override final;

public:
	const float_n vShift = 0.0;
	const float_n hShift = 0.0;
	const float_n scale = 1.0;

	AffineEstimator(
		const EstimatorId id,
		const EstimatorRef<OutU, InU>& base,
		const float_n vShift,
		const float_n hShift,
		const float_n scale
	) noexcept;

	Amount<OutU> get(const Amount<InU> input) const override final;

	const EstimatorRef<OutU, InU>& getBase() const;

	bool isEquivalent(const EstimatorBase& other,
		const float_n epsilon = std::numeric_limits<float_n>::epsilon()
	) const override final;
};


template<Unit OutU, Unit InU>
AffineEstimator<OutU, InU>::AffineEstimator(
	const EstimatorId id,
	const EstimatorRef<OutU, InU>& base,
	const float_n vShift,
	const float_n hShift,
	const float_n scale
) noexcept :
	DerivedEstimator<OutU, InU>(id),
	vShift(vShift),
	hShift(hShift),
	scale(scale),
	base(base)
{}

template<Unit OutU, Unit InU>
Amount<OutU> AffineEstimator<OutU, InU>::get(const Amount<InU> input) const
{
	return base->get(input - hShift) * scale + vShift;
}

template<Unit OutU, Unit InU>
const EstimatorRef<OutU, InU>& AffineEstimator<OutU, InU>::getBase() const
{
	return base;
}

template<Unit OutU, Unit InU>
bool AffineEstimator<OutU, InU>::isEquivalent(const EstimatorBase& other, const float_n epsilon) const
{
	if (EstimatorBase::isEquivalent(other, epsilon) == false)
		return false;

	const auto& oth = static_cast<decltype(*this)&>(other);
	return
		Utils::equal(this->vShift, oth.vShift, epsilon) &&
		Utils::equal(this->hShift, oth.hShift, epsilon) &&
		Utils::equal(this->scale, oth.scale, epsilon) &&
		this->base->isEquivalent(*oth.base);
}

template<Unit OutU, Unit InU>
void AffineEstimator<OutU, InU>::tryPrintOOLDefinition(
	std::ostream& out,
	const bool prettify,
	std::unordered_set<EstimatorId>& alreadyPrinted,
	const uint16_t baseIndent) const
{
	if (alreadyPrinted.contains(EstimatorBase::id))
		return;

	// try to OOL print base, since it might have multiple references
	base->dumpDefinition(out, prettify, alreadyPrinted, false, 0);

	if (this->getRefCount() == 1)
		return;

	alreadyPrinted.emplace(EstimatorBase::id);

	out << '_';
	out << Def::Types::Data;
	out << '<' << EstimatorBase::getDefIdentifier() << '>';

	if (prettify)
	{
		static const uint8_t valueOffset = Utils::max(
			Def::Data::Base.size(),
			Def::Data::VerticalShift.size(),
			Def::Data::HorizontalShift.size(),
			Def::Data::Scale.size()) + 2;

		out << ": " << UnitizedEstimator<OutU, InU>::getUnitSpecifier(true);
		out << " {\n";

		std::string indentStr(baseIndent, ' ');

		out << indentStr << Def::Syntax::Indent << Def::Data::Base << ':' << std::string(valueOffset - Def::Data::Base.size(), ' ');
		base->dumpDefinition(out, true, alreadyPrinted, true, baseIndent + Def::Syntax::Indent.size());

		if (not Utils::equal(vShift, 0.0f))
			out << ",\n" << indentStr << Def::Syntax::Indent << Def::Data::VerticalShift << ':' << std::string(valueOffset - Def::Data::VerticalShift.size(), ' ')
			<< Def::prettyPrint(vShift);
		if (not Utils::equal(hShift, 0.0f))
			out << ",\n" << indentStr << Def::Syntax::Indent << Def::Data::HorizontalShift << ':' << std::string(valueOffset - Def::Data::HorizontalShift.size(), ' ')
			<< Def::prettyPrint(hShift);
		if (not Utils::equal(scale, 1.0f))
			out << ",\n" << indentStr << Def::Syntax::Indent << Def::Data::Scale << ':' << std::string(valueOffset - Def::Data::Scale.size(), ' ')
			<< Def::prettyPrint(scale);

		out << indentStr << "\n};\n\n";
	}
	else
	{
		out << ':' << UnitizedEstimator<OutU, InU>::getUnitSpecifier(prettify);
		out << '{';

		out << Def::Data::Base << ':';
		base->dumpDefinition(out, prettify, alreadyPrinted, true, 0);

		if (not Utils::equal(vShift, 0.0f))
			out << ',' << Def::Data::VerticalShift << ':' << Def::print(vShift);
		if (not Utils::equal(hShift, 0.0f))
			out << ',' << Def::Data::HorizontalShift << ':' << Def::print(hShift);
		if (not Utils::equal(scale, 1.0f))
			out << ',' << Def::Data::Scale << ':' << Def::print(scale);

		out << "};\n";
	}
}

template<Unit OutU, Unit InU>
void AffineEstimator<OutU, InU>::printILDefinition(
	std::ostream& out,
	const bool prettify,
	std::unordered_set<EstimatorId>& alreadyPrinted,
	const uint16_t baseIndent) const
{
	if (alreadyPrinted.contains(EstimatorBase::id))
	{
		out << '$' << EstimatorBase::getDefIdentifier();
		return;
	}

	alreadyPrinted.emplace(EstimatorBase::id);

	if (base->getRefCount() > 1 && not alreadyPrinted.contains(base->getId()))
	{
		Log(this).fatal("Tried to inline-print an estimator sub-definition (id: {0}) with multiple references, id: {1}.",
			Def::print(base->getId()), Def::print(this->id));
	}

	out << '_';
	if (prettify)
	{
		static const uint8_t valueOffset = Utils::max(
			Def::Data::Base.size(),
			Def::Data::VerticalShift.size(),
			Def::Data::HorizontalShift.size(),
			Def::Data::Scale.size()) + 2;

		out << ": " << UnitizedEstimator<OutU, InU>::getUnitSpecifier(true);
		out << " {\n";

		std::string indentStr(baseIndent, ' ');

		out << indentStr << Def::Syntax::Indent << Def::Data::Base << ':' << std::string(valueOffset - Def::Data::Base.size(), ' ');
		base->dumpDefinition(out, true, alreadyPrinted, true, baseIndent + Def::Syntax::Indent.size());

		if (not Utils::equal(vShift, 0.0f))
			out << ",\n" << indentStr << Def::Syntax::Indent << Def::Data::VerticalShift << ':' << std::string(valueOffset - Def::Data::VerticalShift.size(), ' ')
				<< Def::prettyPrint(vShift);
		if (not Utils::equal(hShift, 0.0f))
			out << ",\n" << indentStr << Def::Syntax::Indent << Def::Data::HorizontalShift << ':' << std::string(valueOffset - Def::Data::HorizontalShift.size(), ' ')
				<< Def::prettyPrint(hShift);
		if (not Utils::equal(scale, 1.0f))
			out << ",\n" << indentStr << Def::Syntax::Indent << Def::Data::Scale << ':' << std::string(valueOffset - Def::Data::Scale.size(), ' ')
			<< Def::prettyPrint(scale);

		out << '\n' << indentStr << '}';
	}
	else
	{
		out << ':' << UnitizedEstimator<OutU, InU>::getUnitSpecifier(false);
		out << '{';

		out << Def::Data::Base << ':';
		base->dumpDefinition(out, false, alreadyPrinted, true, 0);

		if (not Utils::equal(vShift, 0.0f))
			out << ',' << Def::Data::VerticalShift << ':' << Def::print(vShift);
		if (not Utils::equal(hShift, 0.0f))
			out << ',' << Def::Data::HorizontalShift << ':' << Def::print(hShift);
		if (not Utils::equal(scale, 1.0f))
			out << ',' << Def::Data::Scale << ':' << Def::print(scale);

		out << '}';
	}
}
