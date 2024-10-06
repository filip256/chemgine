#pragma once

#include "UnitizedEstimator.hpp"
#include "EstimationMode.hpp"
#include "Regressors2D.hpp"

template<typename RegT, Unit OutU, Unit InU>
class Regression2DEstimator : public UnitizedEstimator<OutU, InU>
{
	static_assert(std::is_base_of_v<Regressor2DBase, RegT>,
		"Regression2DEstimator: RegT must be a Regressor2DBase derived type.");

private:
	const EstimationMode mode;
	const RegT regressor;

public:
	Regression2DEstimator(
		const EstimatorId id,
		const RegT& regressor,
		const EstimationMode mode
	) noexcept;

	EstimationMode getMode() const;

	Amount<OutU> get(const Amount<InU> input) const override final;

	const RegT& getRegressor() const;

	bool isEquivalent(const EstimatorBase& other,
		const float epsilon = std::numeric_limits<float>::epsilon()
	) const override final;

	void printDefinition(
		std::ostream& out,
		std::unordered_set<EstimatorId>& alreadyPrinted,
		const bool printInline
	) const override final;
};

template<typename RegT, Unit OutU, Unit InU>
Regression2DEstimator<RegT, OutU, InU>::Regression2DEstimator(
	const EstimatorId id,
	const RegT& regressor,
	const EstimationMode mode
) noexcept :
	UnitizedEstimator<OutU, InU>(id),
	mode(mode),
	regressor(regressor)
{}

template<typename RegT, Unit OutU, Unit InU>
EstimationMode Regression2DEstimator<RegT, OutU, InU>::getMode() const
{
	return mode;
}

template<typename RegT, Unit OutU, Unit InU>
Amount<OutU> Regression2DEstimator<RegT, OutU, InU>::get(const Amount<InU> input) const
{
	return regressor.get(input.asStd());
}

template<typename RegT, Unit OutU, Unit InU>
const RegT& Regression2DEstimator<RegT, OutU, InU>::getRegressor() const
{
	return regressor;
}

template<typename RegT, Unit OutU, Unit InU>
bool Regression2DEstimator<RegT, OutU, InU>::isEquivalent(const EstimatorBase& other, const float epsilon) const
{
	if (not UnitizedEstimator<OutU, InU>::isEquivalent(other, epsilon))
		return false;

	const auto& oth = static_cast<decltype(*this)&>(other);
	return this->regressor.isEquivalent(oth.regressor, epsilon);
}

template<typename RegT, Unit OutU, Unit InU>
void Regression2DEstimator<RegT, OutU, InU>::printDefinition(
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
	out << Keywords::Data::Mode << ':' << Def::print(mode) << ',';
	out << Keywords::Data::Parameters << ':' << Def::print(regressor.getParams());
	out << '}';

	if (not printInline)
		out << ";\n";
}
