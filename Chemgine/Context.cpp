#include "Context.hpp"

Context::Context() noexcept
{
	Atom::setDataStore(dataStore);
	Reactable::setDataStore(dataStore);
	Reactor::setDataStore(dataStore);
	Molecule::setDataStore(dataStore);
	BaseLabwareComponent::setDataStore(dataStore);

	dataStore.loadAtomsData("Data/AtomData.csv")
		.loadEstimatorsData("")
		.loadMoleculesData("Data/MoleculeData.csv")
		.loadGenericMoleculesData("Data/GenericMoleculeData.csv")
		.loadReactionsData("Data/ReactionData.csv")
		.loadLabwareData("Data/LabwareData.csv");

	reactantDump = new DumpContainer();
	atmosphere = new Atmosphere(1.0, 760.0,
		{ { Molecule("N#N"), 78.084 }, { Molecule("O=O"), 20.946 } },
		Amount<Unit::LITER>(1000), *reactantDump);
}

Context::~Context() noexcept
{
	while (reactors.size())
	{
		delete reactors.back();
		reactors.pop_back();
	}

	delete atmosphere;
	delete reactantDump;
}

void Context::addReactor(const Amount<Unit::LITER> maxVolume)
{
	reactors.emplace_back(new Reactor(*atmosphere, maxVolume));
}