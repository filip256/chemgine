#pragma once

#include "DataStoreAccessor.hpp"
#include "ReactionData.hpp"

class Reaction
{
private:
	static DataStoreAccessor dataAccessor;

	const ReactionIdType id;

public:
	Reaction(const ReactionIdType id) noexcept;

	static void setDataStore(const DataStore& dataStore);

	ReactionIdType getId() const;

	const ReactionData& data() const;
};