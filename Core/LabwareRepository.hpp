#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include "BaseLabwareData.hpp"
#include "Object.hpp"

class LabwareRepository
{
private:
	std::unordered_map<LabwareId, std::unique_ptr<const BaseLabwareData>> labware;

	bool checkTextureFile(std::string path, const Def::Location& location);

	template <LabwareType T>
	bool add(const LabwareId id, const Def::Object& definition) = delete;

public:
	LabwareRepository() = default;
	LabwareRepository(const LabwareRepository&) = delete;
	LabwareRepository(LabwareRepository&&) = default;

	bool add(const Def::Object& definition);

	bool contains(const LabwareId id) const;
	const BaseLabwareData& at(const LabwareId id) const;

	size_t totalDefinitionCount() const;

	using Iterator = std::unordered_map<LabwareId, std::unique_ptr<const BaseLabwareData>>::const_iterator;
	Iterator begin() const;
	Iterator end() const;

	size_t size() const;
	void clear();
};
