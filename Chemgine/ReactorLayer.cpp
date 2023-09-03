#include "ReactorLayer.hpp"

ReactorLayer::ReactorLayer(const Reactor& owner, const LayerType type) noexcept :
	owner(owner),
	type(type)
{}

void ReactorLayer::add(std::pair<Molecule, double>&& pair)
{
	if (contents.contains(pair.first))
	{
		contents[pair.first] += pair.second;
		return;
	}

	contents.emplace(std::move(pair));
}

void ReactorLayer::add(const Molecule& molecule, const double amount)
{
	if (contents.contains(molecule))
	{
		contents[molecule] += amount;
		return;
	}

	contents.emplace(std::make_pair(molecule, amount));
}

void ReactorLayer::add(ReactorLayer& other)
{
	// ?maybe reserve is a bad idea here?
	//this->contents.reserve(this->contents.bucket_count() + other.contents.size());

	while(other.contents.size())
	{
		auto node = other.contents.extract(other.contents.begin());
		if (this->contents.contains(node.key()))
		{
			this->contents[node.key()] += node.mapped();
			continue;
		}

		this->contents.insert(std::move(node));
	}
}

void ReactorLayer::add(ReactorLayer& other, const double ratio)
{
	if (ratio >= 1.0)
	{
		this->add(other);
		return;
	}

	for (auto const& pair : other.contents)
	{
		this->add(std::make_pair(pair.first, pair.second * ratio));
		other.contents[pair.first] *= 1.0 - ratio;
	}
}

void ReactorLayer::tick()
{

}