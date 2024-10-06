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
		std::unordered_set<EstimatorId>& alreadyPrinted
	) const override final;

	void printILDefinition(
		std::ostream& out,
		std::unordered_set<EstimatorId>& alreadyPrinted
	) const override final;

public:
	const float vShift = 0.0;
	const float hShift = 0.0;
	const float scale = 1.0;

	AffineEstimator(
		const EstimatorId id,
		const EstimatorRef<OutU, InU>& base,
		const float vShift,
		const float hShift,
		const float scale
	) noexcept;

	Amount<OutU> get(const Amount<InU> input) const override final;

	const EstimatorRef<OutU, InU>& getBase() const;

	bool isEquivalent(const EstimatorBase& other,
		const float epsilon = std::numeric_limits<float>::epsilon()
	) const override final;
};


template<Unit OutU, Unit InU>
AffineEstimator<OutU, InU>::AffineEstimator(
	const EstimatorId id,
	const EstimatorRef<OutU, InU>& base,
	const float vShift,
	const float hShift,
	const float scale
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
bool AffineEstimator<OutU, InU>::isEquivalent(const EstimatorBase& other, const float epsilon) const
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
	std::ostream& out, std::unordered_set<EstimatorId>& alreadyPrinted) const
{
	if (alreadyPrinted.contains(EstimatorBase::id))
		return;

	// try to OOL print base, since it might have multiple references
	base->printDefinition(out, alreadyPrinted, false);

	if (this->getRefCount() == 1)
		return;

	alreadyPrinted.emplace(EstimatorBase::id);

	out << '_';
	out << Keywords::Types::Data;
	out << '<' << EstimatorBase::getDefIdentifier() << '>';
	out << ':' << UnitizedEstimator<OutU, InU>::getUnitSpecifier();

	out << '{';
	out << Keywords::Data::Base << ':';
	base->printDefinition(out, alreadyPrinted, true);

	if (not Utils::equal(vShift, 0.0f))
		out << ',' << Keywords::Data::VerticalShift << ':' << Def::print(vShift);
	if (not Utils::equal(hShift, 0.0f))
		out << ',' << Keywords::Data::HorizontalShift << ':' << Def::print(hShift);
	if (not Utils::equal(scale, 1.0f))
		out << ',' << Keywords::Data::Scale << ':' << Def::print(scale);

	out << '}';

	out << ";\n";
}

template<Unit OutU, Unit InU>
void AffineEstimator<OutU, InU>::printILDefinition(
	std::ostream& out, std::unordered_set<EstimatorId>& alreadyPrinted) const
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
	out << ':' << UnitizedEstimator<OutU, InU>::getUnitSpecifier();

	out << '{';
	out << Keywords::Data::Base << ':';
	base->printDefinition(out, alreadyPrinted, true);

	if(not Utils::equal(vShift, 0.0f))
		out << ',' << Keywords::Data::VerticalShift << ':' << Def::print(vShift);
	if (not Utils::equal(hShift, 0.0f))
		out << ',' << Keywords::Data::HorizontalShift << ':' << Def::print(hShift);
	if (not Utils::equal(scale, 1.0f))
		out << ',' << Keywords::Data::Scale << ':' << Def::print(scale);

	out << '}';
}
