#pragma once

#include "AtomData.hpp"
#include "AtomDataTable.hpp"

#include <string>


class DataStore
{
public:
	AtomDataTable atoms;

	DataStore();
	DataStore(const DataStore&) = delete;

	DataStore& loadAtomsData(const std::string& path);
};