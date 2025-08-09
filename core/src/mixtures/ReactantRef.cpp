#include "mixtures/ReactantRef.hpp"

#include "mixtures/kinds/Mixture.hpp"

ReactantRef::ReactantRef(const Reactant& reactant) noexcept :
	owner(reactant.getContainer()->getContent()),
	id(reactant.getId())
{}
