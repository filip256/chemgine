#include "DataStore.hpp"

DataStore::DataStore() : 
	atoms(),
	genericMolecules(),
	estimators(),
	molecules(estimators),
	reactions(molecules)
{}

DataStore& DataStore::loadAtomsData(const std::string& path)
{
	atoms.loadFromFile(path);
	return *this;
}

DataStore& DataStore::loadMoleculesData(const std::string& path)
{
	molecules.loadFromFile(path);
	return *this;
}

DataStore& DataStore::loadGenericMoleculesData(const std::string& path)
{
	genericMolecules.loadFromFile(path);
	return *this;
}

DataStore& DataStore::loadReactionsData(const std::string& path)
{
	reactions.loadFromFile(path);
	return *this;
}

DataStore& DataStore::loadEstimatorsData(const std::string& path)
{
	estimators.loadFromFile(path);
	return *this;
}

DataStore& DataStore::loadLabwareData(const std::string& path)
{
	labware.loadFromFile(path);
	return *this;
}

DataStore& DataStore::saveMoleculesData(const std::string& path)
{
	molecules.saveToFile(path);
	return *this;
}

DataStore& DataStore::saveGenericMoleculesData(const std::string& path)
{
	genericMolecules.saveToFile(path);
	return *this;
}