#pragma once

#include "data/def/DataDumper.hpp"
#include "estimators/kinds/UnitizedEstimator.hpp"

template <Unit OutU, Unit... InUs>
class ConstantEstimator : public UnitizedEstimator<OutU, InUs...>
{
private:
    using Base = UnitizedEstimator<OutU, InUs...>;

    const Amount<OutU> constant;

public:
    ConstantEstimator(const EstimatorId id, const Amount<OutU> constant) noexcept;

    Amount<OutU> get(const Amount<InUs>...) const override final;

    bool isEquivalent(
        const EstimatorBase& other, const float_s epsilon = std::numeric_limits<float_s>::epsilon()) const override final;

    void dumpDefinition(
        std::ostream&                    out,
        const bool                       prettify,
        std::unordered_set<EstimatorId>& alreadyPrinted,
        const bool                       printInline,
        const uint16_t                   baseIndent) const override final;
};

template <Unit OutU, Unit... InUs>
ConstantEstimator<OutU, InUs...>::ConstantEstimator(const EstimatorId id, const Amount<OutU> constant) noexcept :
    Base(id),
    constant(constant)
{}

template <Unit OutU, Unit... InUs>
Amount<OutU> ConstantEstimator<OutU, InUs...>::get(const Amount<InUs>...) const
{
    return constant;
}

template <Unit OutU, Unit... InUs>
bool ConstantEstimator<OutU, InUs...>::isEquivalent(const EstimatorBase& other, const float_s epsilon) const
{
    if (not EstimatorBase::isEquivalent(other, epsilon))
        return false;

    const auto& oth = static_cast<decltype(*this)&>(other);
    return this->constant.equals(oth.constant, epsilon);
}

template <Unit OutU, Unit... InUs>
void ConstantEstimator<OutU, InUs...>::dumpDefinition(
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

    def::DataDumper dump(out, checked_cast<uint8_t>(def::Data::Constant.size()), baseIndent, prettify);
    if (printInline)
        dump.header("", Base::getUnitSpecifier(), "");
    else
        dump.header(def::Types::Data, Base::getUnitSpecifier(), Base::getDefIdentifier());

    dump.beginProperties().property(def::Data::Constant, constant).endProperties();

    if (not printInline)
        dump.endDefinition();
}
