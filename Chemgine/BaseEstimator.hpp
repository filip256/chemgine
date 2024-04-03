#pragma once

#include "EstimatorType.hpp"

#include <string>

typedef uint16_t EstimatorId;

class EstimatorRepository;

class BaseEstimator
{
protected:
	EstimatorId id;

	BaseEstimator(const EstimatorId id) noexcept;
	BaseEstimator(const BaseEstimator&) = default;
	BaseEstimator(BaseEstimator&&) = default;

public:
	EstimatorId getId() const;

	virtual double get(const double input) const;
	virtual double get(const double input1, const double input2) const;

	virtual bool isEquivalent(const BaseEstimator& other,
		const double epsilon = std::numeric_limits<double>::epsilon()
	) const;

	virtual BaseEstimator* clone() const = 0;

	friend class EstimatorRepository;


	// for memory leak checking 
	static size_t instanceCount;
#ifndef NDEBUG
	void* operator new(const size_t count);
	void operator delete(void* ptr);
#endif
};
