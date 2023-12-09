#pragma once

#include "ApproximatorType.hpp"

#include <string>

typedef uint16_t ApproximatorIdType;

class BaseApproximator
{
protected:
	BaseApproximator(
		const ApproximatorIdType id,
		const std::string& name,
		const ApproximatorType type
	) noexcept;

public:
	const ApproximatorIdType id;
	const ApproximatorType type;
	const std::string name;

	virtual double execute(const double input) const = 0;


	// for memory leak checking 
	static size_t instanceCount;
#ifndef NDEBUG
	void* operator new(const size_t count);
	void operator delete(void* ptr);
#endif
};