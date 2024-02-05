#pragma once

#include "DataStore.hpp"
#include "DumpContainer.hpp"
#include "Atmosphere.hpp"
#include "Reactor.hpp"
#include "LabwareSystem.hpp"

class Context
{
private:
	DataStore dataStore;
	DumpContainer* reactantDump;
	Atmosphere* atmosphere;
	std::vector<Reactor*> reactors;

public:
	Context() noexcept;
	Context(const Context&) = delete;
	Context(Context&&) = delete;
	~Context() noexcept;

	void addReactor(const Amount<Unit::LITER> maxVolume);
};