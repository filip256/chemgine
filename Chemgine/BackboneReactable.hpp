#pragma once

#include "Reactable.hpp"

class BackboneReactable : public Reactable
{
private:
	const ComponentIdType id;

public:
	BackboneReactable(const ComponentIdType id) noexcept;

	const MolecularStructure& getStructure() const override final;
};