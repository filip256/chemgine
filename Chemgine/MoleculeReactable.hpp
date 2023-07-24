#pragma once

#include "Reactable.hpp"

class MoleculeReactable : public Reactable
{
private:
	const ComponentIdType id;

public:
	MoleculeReactable(const ComponentIdType id) noexcept;

	const MolecularStructure& getStructure() const override final;
};