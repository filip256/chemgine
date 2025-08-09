#pragma once

#include "DerivedEstimator.hpp"
#include "data/def/DataDumper.hpp"
#include "data/def/Keywords.hpp"
#include "data/def/Printers.hpp"

template<Unit OutU, Unit InU>
class AffineEstimator : public DerivedEstimator<EstimatorRef<OutU, InU>, OutU, InU>
{
private:
	using Base = DerivedEstimator<EstimatorRef<OutU, InU>, OutU, InU>;

public:
	const float_s vShift = 0.0;
	const float_s hShift = 0.0;
	const float_s scale = 1.0;

	AffineEstimator(
		const EstimatorId id,
		const EstimatorRef<OutU, InU>& base,
		const float_s vShift,
		const float_s hShift,
		const float_s scale
	) noexcept;

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
AffineEstimator<OutU, InU>::AffineEstimator(
	const EstimatorId id,
	const EstimatorRef<OutU, InU>& base,
	const float_s vShift,
	const float_s hShift,
	const float_s scale
) noexcept :
	Base(id, base),
	vShift(vShift),
	hShift(hShift),
	scale(scale)
{}

template<Unit OutU, Unit InU>
Amount<OutU> AffineEstimator<OutU, InU>::get(const Amount<InU> input) const
{
	return Base::getBase()->get(input - hShift) * scale + vShift;
}

template<Unit OutU, Unit InU>
bool AffineEstimator<OutU, InU>::isEquivalent(const EstimatorBase& other, const float_s epsilon) const
{
	if (EstimatorBase::isEquivalent(other, epsilon) == false)
		return false;

	const auto& oth = static_cast<decltype(*this)&>(other);
	return
		utils::floatEqual(this->vShift, oth.vShift, epsilon) &&
		utils::floatEqual(this->hShift, oth.hShift, epsilon) &&
		utils::floatEqual(this->scale, oth.scale, epsilon) &&
		Base::isEquivalent(oth, epsilon);
}

template<Unit OutU, Unit InU>
void AffineEstimator<OutU, InU>::dumpDefinition(
	std::ostream& out,
	const bool prettify,
	std::unordered_set<EstimatorId>& alreadyPrinted,
	const bool printInline,
	const uint16_t baseIndent) const
{
	if (alreadyPrinted.contains(Base::id))
	{
		if(printInline)
			out << '$' << Base::getDefIdentifier();
		return;
	}

	static const auto valueOffset = checked_cast<uint8_t>(utils::max(
		def::Data::Base.size(),
		def::Data::VerticalShift.size(),
		def::Data::HorizontalShift.size(),
		def::Data::Scale.size()));

	const auto& base = Base::getBase();

	def::DataDumper dump(out, valueOffset, baseIndent, prettify);
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
			def::print(base->getId()), def::print(this->id));
	}

	alreadyPrinted.emplace(Base::id);

	if (printInline)
		dump.header("", Base::getUnitSpecifier(), "");
	else
		dump.header(def::Types::Data, Base::getUnitSpecifier(), Base::getDefIdentifier());

	dump.beginProperties()
		.subDefinition(def::Data::Base, base, alreadyPrinted)
		.defaultPropertyWithSep(def::Data::VerticalShift, vShift, 0.0f, true)
		.defaultPropertyWithSep(def::Data::HorizontalShift, hShift, 0.0f, true)
		.defaultPropertyWithSep(def::Data::Scale, scale, 1.0f, true)
		.endProperties();

	if (not printInline)
		dump.endDefinition();
}
