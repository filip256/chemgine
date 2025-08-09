#include "atomics/Bond.hpp"

#include "atomics/BondedAtom.hpp"
#include "io/Log.hpp"

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

char Bond::getASCII(const ASCII::Direction direction) const
{
	return getASCII(type, direction);
}

float_s Bond::getASCIIScore(const ASCII::Direction direction) const
{
	return getASCIIScore(type, direction);
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
		Log<Bond>().fatal("Unsupported bond type: {0}.", underlying_cast(type));
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

char Bond::getASCII(const BondType type, const ASCII::Direction direction)
{
	switch (type)
	{
	case BondType::NON_BOND:
		return 'ù';

	case BondType::IONIC:
	case BondType::SINGLE:
	case BondType::LEVO_SINGLE:
	case BondType::DEXTRO_SINGLE:
		return direction.getSymbol();

	case BondType::DOUBLE:
		if (direction.get().x != 0)
			return 'Í';
		return 'º';

	case BondType::TRIPLE:
		return 'ð';

	case BondType::QUADRUPLE:
		return 'î';

	case BondType::AROMATIC:
		return ':';

	default:
		Log<Bond>().fatal("Unsupported bond type: {0}.", underlying_cast(type));
		return '?';
	}
}

BondType Bond::fromASCII(const char symbol)
{
	switch (symbol)
	{
	case 'ù':
	case '.':
		return BondType::NON_BOND;

	case '³':
	case 'Ä':
	case '/':
	case '\\':
	case '-':
	case '|':
		return BondType::SINGLE;

	case 'Í':
	case 'º':
	case '=':
		return BondType::DOUBLE;

	case 'ð':
		return BondType::TRIPLE;

	case 'î':
		return BondType::QUADRUPLE;

	case ':':
		return BondType::AROMATIC;

	default:
		return BondType::NONE;
	}
}

bool Bond::isInDirection(const char symbol, const ASCII::Direction direction)
{
	if (fromASCII(symbol) != BondType::SINGLE)
		return true;

	switch (direction.getIdx())
	{
	case ASCII::Direction::UpLeft.getIdx():
	case ASCII::Direction::DownRight.getIdx():
		return symbol == '\\';

	case ASCII::Direction::Up.getIdx():
	case ASCII::Direction::Down.getIdx():
		return symbol == '³' || symbol == '|';

	case ASCII::Direction::UpRight.getIdx():
	case ASCII::Direction::DownLeft.getIdx():
		return symbol == '/';

	case ASCII::Direction::Right.getIdx():
	case ASCII::Direction::Left.getIdx():
		return symbol == 'Ä' || symbol == '-';

	default:
		Log<Bond>().fatal("Unsupported direction: {0}.", direction.get());
		return false;
	}
}

bool Bond::hasCompleteASCIIRepresentation(const BondType type)
{
	// Only single bond types can be represented with regular ASCII characters in all the 8 directions.
	switch (type)
	{
	case BondType::IONIC:
	case BondType::SINGLE:
	case BondType::LEVO_SINGLE:
	case BondType::DEXTRO_SINGLE:
		return true;

	case BondType::NON_BOND:
	case BondType::DOUBLE:
	case BondType::TRIPLE:
	case BondType::QUADRUPLE:
		return false;

	default:
		Log<Bond>().fatal("Unsupported bond type: {0}.", underlying_cast(type));
		return false;
	}
}

float_s Bond::getASCIIScore(const BondType type, const ASCII::Direction direction)
{
	switch (type)
	{
	case BondType::NON_BOND:
	case BondType::IONIC:
	case BondType::SINGLE:
	case BondType::LEVO_SINGLE:
	case BondType::DEXTRO_SINGLE:
	case BondType::AROMATIC:
		return 1.0f;

	case BondType::DOUBLE:
	{
		const auto dVector = direction.get();
		if (dVector.x == 0 || dVector.y == 0)
			return 1.0f;
		return -0.7f;
	}

	case BondType::TRIPLE:
	case BondType::QUADRUPLE:
	{
		const auto dVector = direction.get();
		if (dVector.y == 0)
			return 1.0f;
		if (dVector.x == 0)
			return 0.5f;
		return -0.5f;
	}

	default:
		Log<Bond>().fatal("Unsupported bond type: {0}.", underlying_cast(type));
		return 0.0;
	}
}

uint8_t Bond::getValence(const BondType type)
{
	switch (type)
	{
	case BondType::NON_BOND:
	case BondType::IONIC:
	case BondType::SINGLE:
	case BondType::LEVO_SINGLE:
	case BondType::DEXTRO_SINGLE:
	case BondType::AROMATIC:
		return 1;

	case BondType::DOUBLE:
		return 2;

	case BondType::TRIPLE:
		return 3;

	case BondType::QUADRUPLE:
		return 4;

	default:
		Log<Bond>().fatal("Unsupported bond type: {0}.", underlying_cast(type));
		return 0;
	}
}
