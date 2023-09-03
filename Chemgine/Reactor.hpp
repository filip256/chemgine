#pragma once

#include "ReactorLayer.hpp"
#include "DataStoreAccessor.hpp"

class Reactor
{
private:
	std::vector<ReactorLayer> layers;

	static DataStoreAccessor dataAccessor;

	void checkReactions();

public:
	Reactor() noexcept;
	Reactor(const Reactor&) = delete;
	Reactor(Reactor&&) = default;

	const static uint8_t layerCount = static_cast<uint8_t>(LayerType::NONE);

	void add(Reactor& other);
	void add(Reactor& other, const double ratio);

	void tick();

	static void setDataStore(const DataStore& dataStore);

	friend class ReactorLayer;
};