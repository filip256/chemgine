#include "Reactor.hpp"
#include "DataStore.hpp"
#include "Query.hpp"

DataStoreAccessor Reactor::dataAccessor = DataStoreAccessor();

Reactor::Reactor() noexcept
{
	dataAccessor.crashIfUninitialized();

	layers.reserve(layerCount);
	for (uint8_t i = 0; i < layerCount; ++i)
		layers.emplace_back(ReactorLayer(*this, static_cast<LayerType>(i)));
}

void Reactor::setDataStore(const DataStore& dataStore)
{
	dataAccessor.set(dataStore);
}

void Reactor::checkReactions()
{
	for (size_t i = 0; i < layers.size(); ++i)
	{
		for (size_t j = 0; j < layers[i].contents.size(); ++j)
		{
			dataAccessor.get().reactions.findReactionsFor(layers[i].contents[j]))
		}
	}
}

void Reactor::add(Reactor& other)
{
	for (uint8_t i = 0; i < layers.size(); ++i)
		this->layers[i].add(other.layers[i]);
}

void Reactor::add(Reactor& other, const double ratio)
{
	if (ratio >= 1.0)
	{
		this->add(other);
		return;
	}

	for (uint8_t i = 0; i < layers.size(); ++i)
		this->layers[i].add(other.layers[i], ratio);
}

void Reactor::tick()
{
	checkReactions();
}