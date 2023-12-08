#include "DataStore.hpp"

DataStore::DataStore() : 
	atoms(),
	functionalGroups(),
	backbones(),
	molecules(),
	reactions(functionalGroups, backbones, molecules)
{}

DataStore& DataStore::loadAtomsData(const std::string& path)
{
	atoms.loadFromFile(path);
	return *this;
}

DataStore& DataStore::loadFunctionalGroupsData(const std::string& path)
{
	functionalGroups.loadFromFile(path);
	return *this;
}

DataStore& DataStore::loadBackbonesData(const std::string& path)
{
	backbones.loadFromFile(path);
	return *this;
}

DataStore& DataStore::loadMoleculesData(const std::string& path)
{
	molecules.loadFromFile(path);
	return *this;
}

DataStore& DataStore::loadReactionsData(const std::string& path)
{
	reactions.loadFromFile(path);
	return *this;
}

DataStore& DataStore::loadApproximatorsData(const std::string& path)
{
	approximators.loadFromFile(path);
	return *this;
}

DataStore& DataStore::loadLabwareData(const std::string& path)
{
	labware.loadFromFile(path);
	return *this;
}