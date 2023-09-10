#pragma once

#include "Reactable.hpp"

class BackboneReactable : public Reactable
{
private:
	const ComponentIdType id;

public:
	BackboneReactable(const ComponentIdType id) noexcept;

	const MolecularStructure& getStructure() const override final;

	std::unordered_map<c_size, c_size> matchWith(const MolecularStructure& structure) const override final;
};