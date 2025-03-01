#include "Bond.hpp"
#include "BondedAtom.hpp"

Bond::Bond(BondedAtomBase& other, const BondType type) :
	other(&other),
	type(type)
{}

const BondedAtomBase& Bond::getOther() const
{
	return *other;
}

BondedAtomBase& Bond::getOther()
{
	return *other;
}

void Bond::setOther(BondedAtomBase& other)
{
	this->other = &other;
}

BondType Bond::getType() const
{
	return type;
}

uint8_t Bond::getValence() const
{
	return Bond::getValence(type);
}

std::string Bond::getSMILES() const
{
	return getSMILES(type);
}

std::string Bond::getSMILES(const BondType type)
{
	switch (type)
	{
	case BondType::NON_BOND:
		return ".";
	case BondType::SINGLE:
		return "";
	case BondType::DOUBLE:
		return "=";
	case BondType::TRIPLE:
		return "#";
	case BondType::QUADRUPLE:
		return "$";
	case BondType::AROMATIC:
		return ":";
	default:
		return "?";
	}
}

BondType Bond::fromSMILES(const char symbol)
{
	switch (symbol)
	{
	case '.':
		return BondType::NON_BOND;
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
