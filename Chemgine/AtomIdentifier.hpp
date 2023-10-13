#pragma once

#include <vector>

#include "Atom.hpp"
#include "AtomData.hpp"
#include "AtomicComponent.hpp"
#include "CompositeComponent.hpp"

class AtomIdentifier : public AtomicComponent
{
private:
	const CompositeComponent& composite;
	const std::vector<ComponentIdType> ids;

	//AtomIdentifier(const AtomIdentifier&) = default;

public:
	AtomIdentifier(const CompositeComponent& composite);

	ComponentIdType atom() const;

	const AtomData& data() const override final;

	bool isRadicalType() const override final;

	//AtomIdentifier* clone() const override final;
};