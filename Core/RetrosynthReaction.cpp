#include "RetrosynthReaction.hpp"
#include "TextBlock.hpp"

RetrosynthReaction::RetrosynthReaction(
	const ReactionData& baseReaction,
	const std::vector<StructureRef>& reactants,
	const std::vector<StructureRef>& products
) noexcept :
	baseReaction(baseReaction),
	reactants(Utils::aggregate<StructureRef, uint8_t>(reactants)),
	products(Utils::aggregate<StructureRef, uint8_t>(products))
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
	TextBlock buffer;
	const auto lastR = --reactants.end();
	for(auto r = reactants.begin(); r != lastR; ++r)
	{
		if(r->second > 1)
			buffer.appendRight(std::to_string(r->second) + "x ");
		buffer.appendRight(r->first.getStructure().print());
		buffer.appendRight(" + ");
	}
	if (lastR->second > 1)
		buffer.appendRight(std::to_string(lastR->second) + "x ");
	buffer.appendRight(lastR->first.getStructure().print());

	if(baseReaction.isCutReaction())
		buffer.appendRight(" �X�> ");
	else
		buffer.appendRight(" ���> ");

	const auto lastP = --products.end();
	for (auto p = products.begin(); p != lastP; ++p)
	{
		if (p->second > 1)
			buffer.appendRight(std::to_string(p->second) + "x ");
		buffer.appendRight(p->first.getStructure().print());
		buffer.appendRight(" + ");
	}
	if (lastP->second > 1)
		buffer.appendRight(std::to_string(lastP->second) + "x ");
	buffer.appendRight(lastP->first.getStructure().print());

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
