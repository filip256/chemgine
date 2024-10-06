#pragma once

#include "UnitizedEstimator.hpp"
#include "EstimationMode.hpp"
#include "Regressors3D.hpp"

template<typename RegT, Unit OutU, Unit InU1, Unit InU2>
class Regression3DEstimator : public UnitizedEstimator<OutU, InU1, InU2>
{
	static_assert(std::is_base_of_v<Regressor3DBase, RegT>,
		"Regression2DEstimator: RegT must be a Regressor3DBase derived type.");

private:
	const EstimationMode mode;
	const RegT regressor;

public:
	Regression3DEstimator(
		const EstimatorId id,
		const RegT& regressor,
		const EstimationMode mode
	) noexcept;

	EstimationMode getMode() const;

	Amount<OutU> get(const Amount<InU1> input1, const Amount<InU2> input2) const override final;

	bool isEquivalent(const EstimatorBase& other,
		const float epsilon = std::numeric_limits<float>::epsilon()
	) const override final;

	void printDefinition(
		std::ostream& out,
		std::unordered_set<EstimatorId>& alreadyPrinted,
		const bool printInline
	) const override final;
};

template<typename RegT, Unit OutU, Unit InU1, Unit InU2>
Regression3DEstimator<RegT, OutU, InU1, InU2>::Regression3DEstimator(
	const EstimatorId id,
	const RegT& regressor,
	const EstimationMode mode
) noexcept :
	UnitizedEstimator<OutU, InU1, InU2>(id),
	mode(mode),
	regressor(regressor)
{}

template<typename RegT, Unit OutU, Unit InU1, Unit InU2>
EstimationMode Regression3DEstimator<RegT, OutU, InU1, InU2>::getMode() const
{
	return mode;
}

template<typename RegT, Unit OutU, Unit InU1, Unit InU2>
Amount<OutU> Regression3DEstimator<RegT, OutU, InU1, InU2>::get(const Amount<InU1> input1, const Amount<InU2> input2) const
{
	return regressor.get(input1.asStd(), input2.asStd());
}

template<typename RegT, Unit OutU, Unit InU1, Unit InU2>
bool Regression3DEstimator<RegT, OutU, InU1, InU2>::isEquivalent(const EstimatorBase& other, const float epsilon) const
{
	if (not UnitizedEstimator<OutU, InU1, InU2>::isEquivalent(other, epsilon))
		return false;

	const auto& oth = static_cast<decltype(*this)&>(other);
	return this->regressor.isEquivalent(oth.regressor, epsilon);
}

template<typename RegT, Unit OutU, Unit InU1, Unit InU2>
void Regression3DEstimator<RegT, OutU, InU1, InU2>::printDefinition(
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
	out << ':' << UnitizedEstimator<OutU, InU1, InU2>::getUnitSpecifier();

	out << '{';
	out << Keywords::Data::Mode << ':' << Def::print(mode) << ',';
	out << Keywords::Data::Parameters << ':' << Def::print(regressor.getParams());
	out << '}';

	if (not printInline)
		out << ";\n";
}
