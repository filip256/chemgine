#pragma once

#include "BaseApproximator.hpp"

#include <unordered_map>

class ApproximatorDataTable
{
private:
	std::unordered_map<ApproximatorIdType, const BaseApproximator*> table;

public:
	ApproximatorDataTable() = default;
	ApproximatorDataTable(const ApproximatorDataTable&) = delete;
	~ApproximatorDataTable() noexcept;

	bool loadFromFile(const std::string& path);

	const BaseApproximator& at(const ApproximatorIdType id) const;
};