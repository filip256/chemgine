#pragma once

#include "estimators/EstimationMode.hpp"
#include "estimators/kinds/UnitizedEstimator.hpp"

template <typename RegT, Unit... InUs>
concept IsRegressor = requires (RegT reg, const Amount<InUs>... inputs) {
    { reg.get(inputs.asStd()...) };
};

template <typename RegT, Unit OutU, Unit... InUs>
class RegressionEstimator : public UnitizedEstimator<OutU, InUs...>
{
    static_assert(IsRegressor<RegT, InUs...>, "RegressionEstimator: RegT must be a regressor type of the right order.");

protected:
    const RegT regressor;

public:
    using Base = UnitizedEstimator<OutU, InUs...>;

public:
    RegressionEstimator(const EstimatorId id, const RegT& regressor) noexcept;

    EstimationMode getMode() const;
    const RegT&    getRegressor() const;

    Amount<OutU> get(const Amount<InUs>... inputs) const override final;

    bool isEquivalent(
        const EstimatorBase& other, const float_s epsilon = std::numeric_limits<float_s>::epsilon()) const override final;

    void dumpDefinition(
        std::ostream&                    out,
        const bool                       prettify,
        std::unordered_set<EstimatorId>& alreadyPrinted,
        const bool                       printInline,
        const uint16_t                   baseIndent) const override final;
};

template <typename RegT, Unit OutU, Unit... InUs>
RegressionEstimator<RegT, OutU, InUs...>::RegressionEstimator(const EstimatorId id, const RegT& regressor) noexcept :
    Base(id),
    regressor(regressor)
{}

template <typename RegT, Unit OutU, Unit... InUs>
EstimationMode RegressionEstimator<RegT, OutU, InUs...>::getMode() const
{
    return RegT::Mode;
}

template <typename RegT, Unit OutU, Unit... InUs>
const RegT& RegressionEstimator<RegT, OutU, InUs...>::getRegressor() const
{
    return regressor;
}

template <typename RegT, Unit OutU, Unit... InUs>
Amount<OutU> RegressionEstimator<RegT, OutU, InUs...>::get(const Amount<InUs>... inputs) const
{
    return regressor.get(inputs.asStd()...);
}

template <typename RegT, Unit OutU, Unit... InUs>
bool RegressionEstimator<RegT, OutU, InUs...>::isEquivalent(const EstimatorBase& other, const float_s epsilon) const
{
    if (not EstimatorBase::isEquivalent(other, epsilon))
        return false;

    const auto& oth = static_cast<decltype(*this)&>(other);
    return this->regressor.isEquivalent(oth.regressor, epsilon);
}

template <typename RegT, Unit OutU, Unit... InUs>
void RegressionEstimator<RegT, OutU, InUs...>::dumpDefinition(
    std::ostream&                    out,
    const bool                       prettify,
    std::unordered_set<EstimatorId>& alreadyPrinted,
    const bool                       printInline,
    const uint16_t                   baseIndent) const
{
    if (not printInline && this->getRefCount() == 1)
        return;

    if (alreadyPrinted.contains(Base::id)) {
        if (printInline)
            out << '$' << Base::getDefIdentifier();
        return;
    }
    alreadyPrinted.emplace(Base::id);

    static const auto valueOffset = checked_cast<uint8_t>(utils::max(def::Data::Mode.size(), def::Data::Parameters.size()));

    def::DataDumper dump(out, valueOffset, baseIndent, prettify);
    if (printInline)
        dump.header("", Base::getUnitSpecifier(), "");
    else
        dump.header(def::Types::Data, Base::getUnitSpecifier(), Base::getDefIdentifier());

    dump.beginProperties()
        .propertyWithSep(def::Data::Mode, getMode())
        .property(def::Data::Parameters, regressor.getParams())
        .endProperties();

    if (not printInline)
        dump.endDefinition();
}
