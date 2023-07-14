#pragma once

#include "DataTable.hpp"
#include "ReactionData.hpp"

class ReactionDataTable :
	public DataTable<ReactionIdType, std::string, ReactionData>
{
public:
	ReactionDataTable() = default;
	ReactionDataTable(const ReactionDataTable&) = delete;

	bool loadFromFile(const std::string& path);
};