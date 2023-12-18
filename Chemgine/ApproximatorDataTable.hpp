#pragma once

#include "BaseApproximator.hpp"

#include <unordered_map>

class ApproximatorDataTable
{
private:
	std::unordered_map<ApproximatorIdType, const BaseApproximator*> table;

	ApproximatorIdType getFreeId() const;

public:
	ApproximatorDataTable() = default;
	ApproximatorDataTable(const ApproximatorDataTable&) = delete;
	~ApproximatorDataTable() noexcept;

	bool loadFromFile(const std::string& path);
	const BaseApproximator& add(const BaseApproximator& approximator);

	const BaseApproximator& at(const ApproximatorIdType id) const;
};

enum class Approximators : ApproximatorIdType
{
	TEMP_TO_REL_RSPEED = 101,
	MCONC_TO_REL_RSPEED = 102,
	TEMP_TO_DENSITY = 103,
	TORR_TO_REL_BP = 104
};