#include "Bond.hpp"


size_t Bond::instanceCount = 0;

Bond::Bond(const size_t other, const BondType type) :
	other(other),
	type(type)
{}

uint8_t Bond::getValence() const
{
	return Bond::getValence(type);
}

BondType Bond::fromSMILES(const char symbol)
{
	switch (symbol)
	{
	case '.':
		return BondType::NO_BOND;
	case '-':
		return BondType::SINGLE;
	case '=':
		return BondType::DOUBLE;
	case '#':
		return BondType::TRIPLE;
	case '$':
		return BondType::QUADRUPLE;
	case ':':
		return BondType::AROMATIC;
	default:
		return BondType::NONE;
	}
}

uint8_t Bond::getValence(const BondType type)
{
	if (type == BondType::SINGLE)
		return 1;
	if (type == BondType::DOUBLE)
		return 2;
	if (type == BondType::TRIPLE)
		return 3;
	if (type == BondType::QUADRUPLE)
		return 4;

	return 1;
}

#ifndef NDEBUG
void* Bond::operator new(const size_t count)
{
	++instanceCount;
	return ::operator new(count);
}

void Bond::operator delete(void* ptr)
{
	--instanceCount;
	return ::operator delete(ptr);
}
#endif