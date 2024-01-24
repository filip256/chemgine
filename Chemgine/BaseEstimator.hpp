#pragma once

#include "EstimatorType.hpp"

#include <string>

typedef uint16_t EstimatorIdType;

class EstimatorDataTable;

class BaseEstimator
{
protected:
	EstimatorIdType id;

	BaseEstimator(const EstimatorIdType id) noexcept;
	BaseEstimator(const BaseEstimator&) = default;
	BaseEstimator(BaseEstimator&&) = default;

public:
	EstimatorIdType getId() const;

	virtual double get(const double input) const;
	virtual double get(const double input1, const double input2) const;

	virtual BaseEstimator* clone() const = 0;

	friend class EstimatorDataTable;

	// for memory leak checking 
	static size_t instanceCount;
#ifndef NDEBUG
	void* operator new(const size_t count);
	void operator delete(void* ptr);
#endif
};