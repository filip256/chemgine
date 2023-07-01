#include "DataStore.hpp"

DataStore::DataStore() : 
	atoms()
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