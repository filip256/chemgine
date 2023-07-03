#pragma once

#include "CompositeComponentData.hpp"

class FunctionalGroupData : public CompositeComponentData
{
public:
	FunctionalGroupData(
		const ComponentIdType id,
		const std::string& name,
		const std::string& smiles) noexcept;

	FunctionalGroupData(const FunctionalGroupData&) = delete;
	FunctionalGroupData(FunctionalGroupData&&) = default;
	~FunctionalGroupData() = default;
};