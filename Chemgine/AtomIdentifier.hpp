#pragma once

#include "Atom.hpp"
#include "CompositeComponent.hpp"

class AtomIdentifier : public BaseComponent

{
private:
	const CompositeComponent& composite;
	const std::vector<AtomIdType> ids;

public:
	AtomIdentifier(const CompositeComponent& composite);

	const AtomData& data() const override;
};