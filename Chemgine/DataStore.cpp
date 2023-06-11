#include "DataStore.hpp"

DataStore::DataStore() : 
	atomsTable()
{}

DataStore& DataStore::loadAtomsData(const std::string& path)
{
	atomsTable.loadFromFile(path);
	return *this;
}