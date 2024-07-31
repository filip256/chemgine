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

		switch (entry->type)
		{
		case DefinitionType::SPLINE:
			estimators.add<SplineEstimator>(std::move(*entry));
			break;

		case DefinitionType::ATOM:
			atoms.add<AtomData>(std::move(*entry));
			break;

		case DefinitionType::RADICAL:
			atoms.add<RadicalData>(std::move(*entry));
			break;

		case DefinitionType::MOLECULE:
			molecules.add(std::move(*entry));
			break;

		case DefinitionType::REACTION:
			reactions.add(std::move(*entry));
			break;

		case DefinitionType::LABWARE:
			labware.add(std::move(*entry));
			break;
		}
	}
	return *this;
}

DataStore& DataStore::loadAtomsData(const std::string& path)
{
	atoms.loadFromFile(path);
	return *this;
}

DataStore& DataStore::loadMoleculesData(const std::string& path)
{
	molecules.loadFromFile(path);
	return *this;
}

DataStore& DataStore::loadGenericMoleculesData(const std::string& path)
{
	genericMolecules.loadFromFile(path);
	return *this;
}

DataStore& DataStore::loadReactionsData(const std::string& path)
{
	reactions.loadFromFile(path);
	return *this;
}

DataStore& DataStore::loadEstimatorsData(const std::string& path)
{
	estimators.loadFromFile(path);
	return *this;
}

DataStore& DataStore::loadLabwareData(const std::string& path)
{
	labware.loadFromFile(path);
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
