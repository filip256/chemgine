#include "DataStore.hpp"
#include "PathUtils.hpp"
#include "DefFileParser.hpp"
#include "DefFileAnalyzer.hpp"
#include "Log.hpp"

#include <fstream>

DataStore::DataStore() :
	fileStore(),
	oolDefinitions(),
	atoms(),
	genericMolecules(),
	estimators(),
	molecules(estimators),
	reactions(estimators, molecules),
	labware()
{}

DataStore& DataStore::load(const std::string& path)
{
	const auto normPath = Utils::normalizePath(path);

	const auto analysis = DefFileAnalyzer(normPath, fileStore).analyze();
	if (analysis.failed)
		Log(this).warn("Pre-parse analysis failed on file: '{0}'", normPath);
	else
	{
		Log(this).info("Pre-parse analysis on file: '{0}':\n - Total Definitions: {1} ({2} already parsed)\n - Total Files:       {3} ({4} already parsed)",
			normPath, analysis.totalDefinitionCount, analysis.preparsedDefinitionCount, analysis.totalFileCount, analysis.preparsedFileCount);
	}
	
	auto definitionCount = analysis.preparsedDefinitionCount;
	DefFileParser parser(normPath, fileStore, oolDefinitions);
	while (true)
	{
		if (not analysis.failed)
		{
			const auto definitionsToParse = analysis.totalDefinitionCount - analysis.preparsedDefinitionCount;
			const auto perc = static_cast<uint8_t>((static_cast<float>(definitionCount) / definitionsToParse) * 100.f);
			Log(this).info("\r[{0}%] Parsed {1} out of {2} definitions.", perc, definitionCount, definitionsToParse);
		}

		auto entry = parser.nextDefinition();
		if (parser.isOpen() == false)
			break;

		if (not entry)
		{
			Log(this).warn("Skipped invalid definition.");
			continue;
		}

		bool success = false;
		switch (entry->getType())
		{
		case DefinitionType::AUTO:
			Log(this).error("Cannot infer type of out-of-line definition, at: {0}.", entry->getLocationName());
			success = false;
			break;

		case DefinitionType::DATA:
			success = oolDefinitions.add(std::move(*entry));
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
			Log(this).error("Unknown definition type: '{0}', at: {1}.", static_cast<uint8_t>(entry->getType()), entry->getLocationName());
			break;
		}

		if (success == false)
		{
			Log(this).warn("Skipped invalid definition, at: {0}.", entry->getLocationName());
			continue;
		}

		++definitionCount;
	}

	estimators.dropUnusedEstimators();
	oolDefinitions.clear();

	Log(this).success("File load completed.");
	return *this;
}

DataStore& DataStore::dump(const std::string& path)
{
	std::ofstream out(path);
	if (not out.is_open())
		Log(this).fatal("Failed to open file for write: '{0}'.", path);

	out << ":.\n" << "   - Chemgine Output File -\n\n   Version: 0.0.0\n   Sources:\n";

	for (const auto& f : fileStore.getHistory())
		out << "    > " << f.first << '\n';
	out << ".:\n\n";

	for (auto a = atoms.atomsBegin(); a != atoms.atomsEnd(); ++a)
		a->second->printDefinition(out);
	for (auto r = atoms.radicalsBegin(); r != atoms.radicalsEnd(); ++r)
		r->second->printDefinition(out);

	std::unordered_set<EstimatorId> printedEstimators;
	for (const auto& m : molecules)
		m.second->printDefinition(out, printedEstimators);
	for (const auto& r : reactions)
		r.second->printDefinition(out, printedEstimators);

	const auto dir = Utils::extractDirName(path);
	for (const auto& l : labware)
	{
		l.second->printDefinition(out);
		l.second->dumpTextures(dir);
	}

	out.close();
	return *this;
}

DataStore& DataStore::clear()
{
	labware.clear();
	reactions.clear();
	genericMolecules.clear();
	molecules.clear();
	estimators.clear();
	atoms.clear();
	oolDefinitions.clear();
	fileStore.clear();
	return *this;
}

DataStore& DataStore::saveGenericMoleculesData(const std::string& path)
{
	genericMolecules.saveToFile(path);
	return *this;
}
