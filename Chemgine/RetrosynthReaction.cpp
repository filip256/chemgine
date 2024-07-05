#include "RetrosynthReaction.hpp"
#include "Utils.hpp"
#include "TextBlock.hpp"

RetrosynthReaction::RetrosynthReaction(
	const ReactionData& baseReaction,
	const std::vector<Reactable>& reactants,
	const std::vector<Reactable>& products
) noexcept :
	baseReaction(baseReaction),
	reactants(Utils::aggregate<Reactable, uint8_t>(reactants)),
	products(Utils::aggregate<Reactable, uint8_t>(products))
{}

const std::unordered_map<Reactable, uint8_t>& RetrosynthReaction::getReactants() const
{
	return reactants;
}

const std::unordered_map<Reactable, uint8_t>& RetrosynthReaction::getProducts() const
{
	return products;
}

const ReactionData& RetrosynthReaction::getBaseData() const
{
	return baseReaction;
}

std::string RetrosynthReaction::getHRTag() const
{
	return baseReaction.getHRTag();
}

std::string RetrosynthReaction::print() const
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
		buffer.appendRight(" ÄXÄ> ");
	else
		buffer.appendRight(" ÄÄÄ> ");

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

	std::string result = getHRTag() + '\n' + buffer.toString() + '\n';

	// catalysts
	const auto catalysts = baseReaction.getCatalysts();
	if (catalysts.size())
	{
		result += " - Catalysts:  ";
		const auto lastC = --catalysts.end();
		for (auto c = catalysts.begin(); c != lastC; ++c)
			result += c->getHRTag() + ", ";

		result += lastC->getHRTag() + '\n';
	}

	if (baseReaction.isCutReaction())
		return result;

	// properties
	result += " - Activation: " + baseReaction.activationEnergy.toString() + '\n';
	result += " - Speed:      " + baseReaction.baseSpeed.toString() + " at " + baseReaction.baseTemperature.toString() + " (relative)\n";
	result += " - Energy:     " + baseReaction.activationEnergy.toString() + '\n';

	return result;
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
