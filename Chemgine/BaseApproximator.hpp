#pragma once

#include "ApproximatorType.hpp"

#include <string>

typedef uint16_t ApproximatorIdType;

class ApproximatorDataTable;

class BaseApproximator
{
protected:
	ApproximatorIdType id;

	BaseApproximator(
		const ApproximatorIdType id,
		const std::string& name,
		const ApproximatorType type
	) noexcept;
	BaseApproximator(const BaseApproximator&) = default;
	BaseApproximator(BaseApproximator&&) = default;

public:
	BaseApproximator(ApproximatorIdType id, BaseApproximator&& other) noexcept;

	const ApproximatorType type;
	const std::string name;

	ApproximatorIdType getId() const;

	virtual double get(const double input) const;
	virtual double get(const double input1, const double input2) const;

	virtual BaseApproximator* clone() const = 0;

	friend class ApproximatorDataTable;

	// for memory leak checking 
	static size_t instanceCount;
#ifndef NDEBUG
	void* operator new(const size_t count);
	void operator delete(void* ptr);
#endif
};