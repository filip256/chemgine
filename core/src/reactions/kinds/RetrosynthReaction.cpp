#include "reactions/kinds/RetrosynthReaction.hpp"

#include "global/Charset.hpp"
#include "structs/Buffer2D.hpp"

RetrosynthReaction::RetrosynthReaction(
	const ReactionData& baseReaction,
	const std::vector<StructureRef>& reactants,
	const std::vector<StructureRef>& products
) noexcept :
	baseReaction(baseReaction),
	reactants(utils::aggregate<StructureRef, uint8_t>(reactants)),
	products(utils::aggregate<StructureRef, uint8_t>(products))
{}

const std::unordered_map<StructureRef, uint8_t>& RetrosynthReaction::getReactants() const
{
	return reactants;
}

const std::unordered_map<StructureRef, uint8_t>& RetrosynthReaction::getProducts() const
{
	return products;
}

const ReactionData& RetrosynthReaction::getBaseData() const
{
	return baseReaction;
}

void RetrosynthReaction::print(std::ostream& out) const
{
	// reactants and products
	ColoredTextBlock buffer;

	const auto printStructures = [&buffer](const auto& structures)
	{
		size_t i = 0;
		size_t last = structures.size() - 1; // Can't determine the last iterator, so we have to count.
		for(auto s = structures.begin(); s != structures.end(); ++s, ++i)
		{
			if(s->second > 1)
				buffer.appendRight(std::to_string(s->second) + "x ");
			buffer.appendRight(s->first.getStructure().toASCII());
			if(i != last)
				buffer.appendRight(" + ");
		}
	};

	printStructures(reactants);

	if(baseReaction.isCutReaction())
		buffer.appendRight(std::string{' ', ASCII::LineH, 'X', ASCII::LineH, '>', ' '});
	else
		buffer.appendRight(std::string{' ', ASCII::LineH, ASCII::LineH, ASCII::LineH, '>', ' '});

	printStructures(products);

	out << buffer << '\n';
	baseReaction.print(out);
}

bool RetrosynthReaction::operator==(const RetrosynthReaction& other) const
{
	return &this->baseReaction == &other.baseReaction &&
		this->reactants == other.reactants;
}

bool RetrosynthReaction::operator!=(const RetrosynthReaction& other) const
{
	return &this->baseReaction != &other.baseReaction ||
		this->reactants != other.reactants;
}
