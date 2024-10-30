#pragma once

#include "UnitizedEstimator.hpp"
#include "DataDumper.hpp"

template<Unit OutU, Unit... InUs>
class ConstantEstimator : public UnitizedEstimator<OutU, InUs...>
{
private:
	const Amount<OutU> constant;

public:
	ConstantEstimator(
		const EstimatorId id,
		const Amount<OutU> constant
	) noexcept;

	Amount<OutU> get(const Amount<InUs>...) const override final;

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

template<Unit OutU, Unit... InUs>
ConstantEstimator<OutU, InUs...>::ConstantEstimator(
	const EstimatorId id,
	const Amount<OutU> constant
) noexcept :
	UnitizedEstimator<OutU, InUs...>(id),
	constant(constant)
{}

template<Unit OutU, Unit... InUs>
Amount<OutU> ConstantEstimator<OutU, InUs...>::get(const Amount<InUs>...) const
{
	return constant;
}

template<Unit OutU, Unit... InUs>
bool ConstantEstimator<OutU, InUs...>::isEquivalent(const EstimatorBase& other, const float_n epsilon) const
{
	if (not EstimatorBase::isEquivalent(other, epsilon))
		return false;

	const auto& oth = static_cast<decltype(*this)&>(other);
	return this->constant.equals(oth.constant, epsilon);
}

template<Unit OutU, Unit... InUs>
void ConstantEstimator<OutU, InUs...>::dumpDefinition(
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
		if(printInline)
			out << '$' << EstimatorBase::getDefIdentifier();
		return;
	}
	alreadyPrinted.emplace(EstimatorBase::id);

	Def::DataDumper dump(out, Def::Data::Constant.size(), baseIndent, prettify);
	if (printInline)
		dump.header("", UnitizedEstimator<OutU, InUs...>::getUnitSpecifier(), "");
	else
		dump.header(Def::Types::Data, UnitizedEstimator<OutU, InUs...>::getUnitSpecifier(), EstimatorBase::getDefIdentifier());

	dump.beginProperties()
		.property(Def::Data::Constant, constant.asStd())
		.endProperties();

	if (not printInline)
		dump.endDefinition();
}
