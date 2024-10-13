#pragma once

#include "Precision.hpp"
#include "EstimatorType.hpp"
#include "CountedRef.hpp"

#include <string>
#include <unordered_set>

typedef uint16_t EstimatorId;

class EstimatorBase : public Countable<>
{
protected:
	const EstimatorId id;

	EstimatorBase(const EstimatorId id) noexcept;
	EstimatorBase(const EstimatorBase&) = delete;
	EstimatorBase(EstimatorBase&&) = default;

public:
	virtual ~EstimatorBase() = default;

	EstimatorId getId() const;
	std::string getDefIdentifier() const;

	virtual bool isEquivalent(const EstimatorBase& other,
		const float_n epsilon = std::numeric_limits<float_n>::epsilon()
	) const;

	virtual void printDefinition(
		std::ostream& out,
		std::unordered_set<EstimatorId>& alreadyPrinted,
		const bool printInline
	) const = 0;
};
