#pragma once

#include "Reactable.hpp"

class FunctionalGroupReactable : public Reactable
{
private:
	const ComponentIdType id;

public:
	FunctionalGroupReactable(const ComponentIdType id) noexcept;

	const MolecularStructure& getStructure() const override final;
};