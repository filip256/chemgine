#include "Bond.hpp"

Bond::Bond(const Atom& other, const BondType type) :
	other(other),
	type(type)
{}
