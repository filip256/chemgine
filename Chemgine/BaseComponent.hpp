#pragma once
#include "DataStore.hpp"

class BaseComponent
{
protected:
	static const DataStore* dataStore;

public:
	BaseComponent();

	static void setDataStore(const DataStore* const dataStore);
};