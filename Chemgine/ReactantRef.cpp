#include "ReactantRef.hpp"
#include "Mixture.hpp"

ReactantRef::ReactantRef(const Reactant& reactant) noexcept :
	owner(reactant.getContainer()->getContent()),
	id(reactant.getId())
{}
