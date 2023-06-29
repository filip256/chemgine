#include "FunctionalGroupDataTable.hpp"
#include "DataHelpers.hpp"
#include "Logger.hpp"
#include <fstream>

FunctionalGroupDataTable::FunctionalGroupDataTable() : DataTable<ComponentIdType, std::string, FunctionalGroupData>()
{

}

bool FunctionalGroupDataTable::loadFromFile(const std::string& path)
{
	std::ifstream file(path);

	if (!file.is_open())
		return false;

	//if (files::verifyChecksum(file).code != 200) //not OK
	//	return StatusCode<>::FileCorrupted;

	table.clear();

	//parse file
	std::string buffer;
	std::getline(file, buffer);
	while (std::getline(file, buffer))
	{
		//auto line = DataHelpers::parseList(buffer, ',');

		//if (line[1].empty())
		//{
		//	Logger::log("Failed to load atom due to empty symbol string.", LogType::BAD);
		//	continue;
		//}
		//if (line[2].empty())
		//{
		//	Logger::log("Atom name was empty. (" + line[1] + ')', LogType::WARN);
		//}

		//const auto id = DataHelpers::toUInt(line[0]);
		//auto weight = DataHelpers::toUDouble(line[3]);
		//auto valence = DataHelpers::toUInt(line[4]);

		//if (line[3].empty())
		//	weight.result = 0;
		//if (line[4].empty())
		//	valence.result = 0;

		//if (id.status == 0)
		//{
		//	Logger::log("Missing id, atom '" + line[1] + "' skipped.", LogType::BAD);
		//	continue;
		//}

		//if (table.emplace(
		//	id.result,
		//	line[1],
		//	std::move(AtomData(id.result, line[1], line[2], weight.result, valence.result))
		//) == false)
		//{
		//	Logger::log("Possible atom duplicate id value: " + std::to_string(table.size()) + '.', LogType::WARN);
		//}
	}
	file.close();

	return true;
}