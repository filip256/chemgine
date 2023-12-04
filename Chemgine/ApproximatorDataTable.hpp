#pragma once

#include "FunctionalApproximator.hpp"

#include <unordered_map>

class ApproximatorDataTable
{
private:
	std::unordered_map<ApproximatorIdType, FunctionalApproximator> table;

public:
	ApproximatorDataTable() = default;
	ApproximatorDataTable(const ApproximatorDataTable&) = delete;

	bool loadFromFile(const std::string& path);

	const FunctionalApproximator& at(const ApproximatorIdType id) const;
};