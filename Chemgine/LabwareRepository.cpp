#include "LabwareRepository.hpp"
#include "DataHelpers.hpp"
#include "LabwareDataFactory.hpp"
#include "Log.hpp"

#include <fstream>

LabwareRepository::~LabwareRepository() noexcept
{
	for (const auto& l : table)
		delete l.second;
}

bool LabwareRepository::loadFromFile(const std::string& path)
{
	std::ifstream file(path);

	if (!file.is_open())
	{
		Log(this).error("Failed to open file '{0}'.", path);
		return false;
	}

	//if (files::verifyChecksum(file).code != 200) //not OK
	//	return StatusCode<>::FileCorrupted;

	table.clear();

	//parse file
	std::string buffer;
	std::getline(file, buffer);
	while (std::getline(file, buffer))
	{
		auto line = DataHelpers::parseList(buffer, ',');

		if (line.size() < 3)
		{
			Log(this).error("Failed to load labware due to missing id or type.");
			continue;
		}

		const auto id = DataHelpers::parseId<LabwareId>(line[0]);
		const auto type = DataHelpers::parseEnum<LabwareType>(line[1]);

		if (id.has_value() == false || type.has_value() == false)
		{
			Log(this).error("Failed to load labware due to missing id or type.");
			continue;
		}

		const auto ptr = LabwareDataFactory::get(*id, *type, line);
		if(ptr == nullptr)
		{
			Log(this).error("Failed to load labware with id {0}.", *id);
			continue;
		}

		if (table.emplace(*id, ptr).second == false)
		{
			Log(this).warn("Duplicate labware with id {0} skipped.", *id);
		}
	}
	file.close();

	Log(this).info("Loaded {0} labware items.", table.size());

	return true;
}

const BaseLabwareData& LabwareRepository::at(const LabwareId id) const
{
	return *table.at(id);
}
