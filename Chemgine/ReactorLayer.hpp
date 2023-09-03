#pragma once

#include <unordered_map>

#include "Molecule.hpp"
#include "LayerType.hpp"

class Reactor;

class ReactorLayer
{
private:
	const LayerType type;
	const Reactor& owner;
	std::unordered_map<Molecule, double, MoleculeHash> contents;

public:
	ReactorLayer(const Reactor& owner, const LayerType type) noexcept;
	ReactorLayer(const ReactorLayer&) = delete;
	ReactorLayer(ReactorLayer&&) = default;

	void add(const Molecule& molecule, const double amount);
	void add(std::pair<Molecule, double>&& pair);
	void add(ReactorLayer& other);
	void add(ReactorLayer& other, const double ratio);

	void tick();

	friend class Reactor;
};