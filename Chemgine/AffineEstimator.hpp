#pragma once

#include "DerivedEstimator.hpp"
#include "DataDumper.hpp"
#include "Keywords.hpp"
#include "Printers.hpp"

template<Unit OutU, Unit InU>
class AffineEstimator : public DerivedEstimator<OutU, InU>
{
private:
	const EstimatorRef<OutU, InU> base;

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

	void dumpDefinition(
		std::ostream& out,
		const bool prettify,
		std::unordered_set<EstimatorId>& alreadyPrinted,
		const bool printInline,
		const uint16_t baseIndent
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
		Utils::floatEqual(this->vShift, oth.vShift, epsilon) &&
		Utils::floatEqual(this->hShift, oth.hShift, epsilon) &&
		Utils::floatEqual(this->scale, oth.scale, epsilon) &&
		this->base->isEquivalent(*oth.base);
}

template<Unit OutU, Unit InU>
void AffineEstimator<OutU, InU>::dumpDefinition(
	std::ostream& out,
	const bool prettify,
	std::unordered_set<EstimatorId>& alreadyPrinted,
	const bool printInline,
	const uint16_t baseIndent) const
{
	if (alreadyPrinted.contains(EstimatorBase::id))
	{
		if(printInline)
			out << '$' << EstimatorBase::getDefIdentifier();
		return;
	}

	static const uint8_t valueOffset = Utils::max(
		Def::Data::Base.size(),
		Def::Data::VerticalShift.size(),
		Def::Data::HorizontalShift.size(),
		Def::Data::Scale.size());

	Def::DataDumper dump(out, valueOffset, baseIndent, prettify);
	if (not printInline)
	{
		// try to OOL print base, since it might have multiple references
		dump.tryOolSubDefinition(base, alreadyPrinted);
		if (this->getRefCount() == 1)
			return;
	}
	else if(base->getRefCount() > 1 && not alreadyPrinted.contains(base->getId()))
	{
		Log(this).fatal("Tried to inline-print an estimator sub-definition (id: {0}) with multiple references, id: {1}.",
			Def::print(base->getId()), Def::print(this->id));
	}

	alreadyPrinted.emplace(EstimatorBase::id);

	if (printInline)
		dump.header("", UnitizedEstimator<OutU, InU>::getUnitSpecifier(), "");
	else
		dump.header(Def::Types::Data, UnitizedEstimator<OutU, InU>::getUnitSpecifier(), EstimatorBase::getDefIdentifier());

	dump.beginProperties()
		.subDefinition(Def::Data::Base, base, alreadyPrinted)
		.defaultPropertyWithSep(Def::Data::VerticalShift, vShift, 0.0f, true)
		.defaultPropertyWithSep(Def::Data::HorizontalShift, hShift, 0.0f, true)
		.defaultPropertyWithSep(Def::Data::Scale, scale, 1.0f, true)
		.endProperties();

	if (not printInline)
		dump.endDefinition();
}
