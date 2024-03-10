#include "LabwareDataTable.hpp"
#include "DataHelpers.hpp"
#include "LabwareDataFactory.hpp"
#include "Logger.hpp"

#include <fstream>

LabwareDataTable::~LabwareDataTable() noexcept
{
	for (const auto& l : table)
		delete l.second;
}

bool LabwareDataTable::loadFromFile(const std::string& path)
{
	std::ifstream file(path);

	if (!file.is_open())
	{
		Logger::log("Failed to open file '" + path + "'.", LogType::BAD);
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
			Logger::log("Failed to load labware due to missing id or type.", LogType::BAD);
			continue;
		}

		const auto id = DataHelpers::parse<unsigned int>(line[0]);
		const auto type = DataHelpers::parse<unsigned int>(line[1]);

		if (id.has_value() == false || type.has_value() == false)
		{
			Logger::log("Failed to load labware due to missing id or type.", LogType::BAD);
			continue;
		}

		const auto ptr = LabwareDataFactory::get(*id, static_cast<LabwareType>(*type), line);
		if(ptr == nullptr)
		{
			Logger::log("Failed to load labware with id " + std::to_string(*id) + ".", LogType::BAD);
			continue;
		}

		if (table.emplace(*id, ptr).second == false)
		{
			Logger::log("Duplicate labware with id " + std::to_string(*id) + "skipped.", LogType::WARN);
		}
	}
	file.close();

	Logger::log("Loaded " + std::to_string(table.size()) + " labware items.", LogType::INFO);

	return true;
}


const BaseLabwareData& LabwareDataTable::at(const LabwareId id) const
{
	return *table.at(id);
}