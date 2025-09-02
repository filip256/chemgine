#pragma once

#include "global/Precision.hpp"
#include "structs/CountedRef.hpp"

#include <string>
#include <unordered_set>

typedef uint16_t EstimatorId;

class EstimatorBase : public Countable<>
{
protected:
    const EstimatorId id;

    EstimatorBase(const EstimatorId id) noexcept;
    EstimatorBase(const EstimatorBase&) = delete;
    EstimatorBase(EstimatorBase&&)      = default;

public:
    virtual ~EstimatorBase() = default;

    EstimatorId getId() const;
    std::string getDefIdentifier() const;

    virtual bool
    isEquivalent(const EstimatorBase& other, const float_s epsilon = std::numeric_limits<float_s>::epsilon()) const;

    virtual uint16_t getNestingDepth() const;

    virtual void dumpDefinition(
        std::ostream&                    out,
        const bool                       prettify,
        std::unordered_set<EstimatorId>& alreadyPrinted,
        const bool                       printInline,
        const uint16_t                   baseIndent) const = 0;
    void print(std::ostream& out = std::cout) const;
};
