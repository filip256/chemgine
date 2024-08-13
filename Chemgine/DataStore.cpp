#include "DataStore.hpp"
#include "DefFileParser.hpp"
#include "Log.hpp"

#include <fstream>

DataStore::DataStore() : 
	atoms(),
	genericMolecules(),
	estimators(),
	molecules(estimators),
	reactions(molecules),
	labware()
{}

DataStore& DataStore::load(const std::string& path)
{
	DefFileParser parser(path, *this);
	while (true)
	{
		auto entry = parser.nextDefinition();
		if (parser.isOpen() == false)
			break;

		if (entry.has_value() == false)
		{
			Log(this).warn("Skipped invalid definition.");
			continue;
		}

		bool success = false;
		switch (entry->type)
		{
		case DefinitionType::SPLINE:
			success = estimators.addOOLDefinition(std::move(*entry));
			break;

		case DefinitionType::ATOM:
			success = atoms.add<AtomData>(std::move(*entry));
			break;

		case DefinitionType::RADICAL:
			success = atoms.add<RadicalData>(std::move(*entry));
			break;

		case DefinitionType::MOLECULE:
			success = molecules.add(std::move(*entry));
			break;

		case DefinitionType::REACTION:
			success = reactions.add(std::move(*entry));
			break;

		case DefinitionType::LABWARE:
			success = labware.add(std::move(*entry));
			break;

		default:
			Log(this).error("Unknown definition type: '{0}', at: {1}.", static_cast<uint8_t>(entry->type), entry->getLocationName());
			break;
		}

		if (success == false)
		{
			Log(this).warn("Skipped invalid definition, at: {0}.", entry->getLocationName());
			continue;
		}
	}
	return *this;
}

DataStore& DataStore::saveMoleculesData(const std::string& path)
{
	molecules.saveToFile(path);
	return *this;
}

DataStore& DataStore::saveGenericMoleculesData(const std::string& path)
{
	genericMolecules.saveToFile(path);
	return *this;
}

ParseStatus DataStore::getFileStatus(const std::string& filePath)
{
	const auto it = fileParseHistory.find(filePath);
	return it == fileParseHistory.end() ? ParseStatus::UNTOUCHED :
		it->second ? ParseStatus::PARSED :
		ParseStatus::STARTED;
}

bool DataStore::wasParsed(const std::string& filePath)
{
	return getFileStatus(filePath) == ParseStatus::PARSED;
}
