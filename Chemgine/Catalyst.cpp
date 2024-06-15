#include "Catalyst.hpp"

Catalyst::Catalyst(const Reactable& reactable, const Amount<Unit::MOLE_RATIO> idealAmount) noexcept :
	reactable(reactable),
	idealAmount(idealAmount)
{}

const MoleculeId Catalyst::getId() const
{
	return reactable.getId();
}

const MolecularStructure& Catalyst::getStructure() const
{
	return reactable.getStructure();
}

const Amount<Unit::MOLE_RATIO> Catalyst::getIdealAmount() const
{
	return idealAmount;
}

std::unordered_map<c_size, c_size> Catalyst::matchWith(const Catalyst& other) const
{
	return reactable.matchWith(other.reactable);
}

std::unordered_map<c_size, c_size> Catalyst::matchWith(const MolecularStructure& structure) const
{
	return reactable.matchWith(structure);
}

bool Catalyst::matchesWith(const Catalyst& other) const
{
	return this->idealAmount == other.idealAmount &&
		this->matchWith(other).size();
}

bool Catalyst::matchesWith(const MolecularStructure& structure) const
{
	return this->matchWith(structure).size();
}

bool Catalyst::operator==(const Catalyst& other) const
{
	return this->reactable == other.reactable;
}

bool Catalyst::operator!=(const Catalyst& other) const
{
	return this->reactable != other.reactable;
}

std::optional<Catalyst> Catalyst::get(
	const std::string& smiles,
	const Amount<Unit::MOLE_RATIO> idealAmount)
{
	const auto r = Reactable::get(smiles);
	if (r.has_value())
		return Catalyst(*r, idealAmount);

	return std::nullopt;
}
