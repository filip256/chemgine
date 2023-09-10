#pragma once

#include "Reaction.hpp"

class ConcreteReaction
{
private:
	const Reaction baseReaction;
	std::vector<std::pair<Molecule, uint8_t>> reactants;
	std::vector<std::pair<Molecule, uint8_t>> products;

public:
};