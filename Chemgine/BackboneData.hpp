#pragma once

#include "CompositeComponentData.hpp"

class BackboneData : public CompositeComponentData
{
public:
	BackboneData(
		const ComponentIdType id,
		const std::string& name,
		const std::string& smiles) noexcept;

	BackboneData(const BackboneData&) = delete;
	BackboneData(BackboneData&&) = default;
	~BackboneData() = default;
};