#pragma once

#include "UnitizedEstimator.hpp"
#include "DataDumper.hpp"

template<UnitType OutU, UnitType... InUs>
class ConstantEstimator : public UnitizedEstimator<OutU, InUs...>
{
private:
	using Base = UnitizedEstimator<OutU, InUs...>;

	const Quantity<OutU> constant;

public:
	ConstantEstimator(
		const EstimatorId id,
		const Quantity<OutU> constant
	) noexcept;

	Quantity<OutU> get(const Quantity<InUs>...) const override final;

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

template<UnitType OutU, UnitType... InUs>
ConstantEstimator<OutU, InUs...>::ConstantEstimator(
	const EstimatorId id,
	const Quantity<OutU> constant
) noexcept :
	Base(id),
	constant(constant)
{}

template<UnitType OutU, UnitType... InUs>
Quantity<OutU> ConstantEstimator<OutU, InUs...>::get(const Quantity<InUs>...) const
{
	return constant;
}

template<UnitType OutU, UnitType... InUs>
bool ConstantEstimator<OutU, InUs...>::isEquivalent(const EstimatorBase& other, const float_s epsilon) const
{
	if (not EstimatorBase::isEquivalent(other, epsilon))
		return false;

	const auto& oth = static_cast<decltype(*this)&>(other);
	return this->constant.equals(oth.constant, epsilon);
}

template<UnitType OutU, UnitType... InUs>
void ConstantEstimator<OutU, InUs...>::dumpDefinition(
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
		if(printInline)
			out << '$' << Base::getDefIdentifier();
		return;
	}
	alreadyPrinted.emplace(Base::id);

	Def::DataDumper dump(out, checked_cast<uint8_t>(Def::Data::Constant.size()), baseIndent, prettify);
	if (printInline)
		dump.header("", Base::getUnitSpecifier(), "");
	else
		dump.header(Def::Types::Data, Base::getUnitSpecifier(), Base::getDefIdentifier());

	dump.beginProperties()
		.property(Def::Data::Constant, constant)
		.endProperties();

	if (not printInline)
		dump.endDefinition();
}
