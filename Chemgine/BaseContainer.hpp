#pragma once

#include "Amount.hpp"

class Reactant;

/// <summary>
/// Abstract base for reactant containers.
/// </summary>
class BaseContainer
{
public:
	virtual ~BaseContainer() = default;

	virtual void add(const Reactant& reactant) = 0;
	virtual void add(const Amount<Unit::JOULE> energy) = 0;


	// for memory leak checking 
	static size_t instanceCount;
#ifndef NDEBUG
	void* operator new(const size_t count);
	void operator delete(void* ptr);
#endif
};
