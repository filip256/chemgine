#pragma once

#include "EstimatorType.hpp"

#include <string>

typedef uint16_t EstimatorId;

class EstimatorRepository;

class EstimatorBase
{
protected:
	EstimatorId id;

	EstimatorBase(const EstimatorId id) noexcept;
	EstimatorBase(const EstimatorBase&) = default;
	EstimatorBase(EstimatorBase&&) = default;

public:
	virtual ~EstimatorBase() = default;

	EstimatorId getId() const;

	virtual bool isEquivalent(const EstimatorBase& other,
		const double epsilon = std::numeric_limits<double>::epsilon()
	) const;

	virtual EstimatorBase* clone() const = 0;

	friend class EstimatorRepository;


	// for memory leak checking 
	static size_t instanceCount;
#ifndef NDEBUG
	void* operator new(const size_t count);
	void operator delete(void* ptr);
#endif
};
