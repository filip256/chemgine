#include "data/DataStore.hpp"

#include "data/def/FileAnalyzer.hpp"
#include "data/def/FileParser.hpp"
#include "io/Log.hpp"
#include "utils/Path.hpp"

#include <fstream>

DataStore::DataStore() :
    fileStore(),
    outlineDefinitions(),
    atoms(),
    estimators(),
    molecules(estimators),
    reactions(estimators, molecules),
    labware()
{}

size_t DataStore::totalDefinitionCount() const
{
    return outlineDefinitions.totalDefinitionCount() +
           atoms.totalDefinitionCount() +
           estimators.totalDefinitionCount() +
           molecules.totalDefinitionCount() +
           reactions.totalDefinitionCount() +
           labware.totalDefinitionCount();
}

bool DataStore::addDefinition(def::Object&& definition)
{
    switch (definition.getType()) {
    case def::DefinitionType::AUTO:
        Log(this).error("Cannot infer type for out-of-line definition, at: {0}.", definition.getLocationName());
        return false;

    case def::DefinitionType::DATA:
        return outlineDefinitions.add(std::move(definition));
    case def::DefinitionType::ATOM:
        return atoms.add<AtomData>(definition);
    case def::DefinitionType::RADICAL:
        return atoms.add<RadicalData>(definition);
    case def::DefinitionType::MOLECULE:
        return molecules.add(definition);
    case def::DefinitionType::REACTION:
        return reactions.add(definition);
    case def::DefinitionType::LABWARE:
        return labware.add(definition);

    default:
        Log(this).error(
            "Unknown definition type: '{0}', at: {1}.",
            static_cast<uint8_t>(definition.getType()),
            definition.getLocationName());
        return false;
    }
}

bool DataStore::load(const std::string& path)
{
    const auto normPath = utils::normalizePath(path);

    const auto analysis = def::FileAnalyzer(normPath, fileStore).analyze();
    if (analysis.failed)
        Log(this).warn("Pre-parse analysis failed on file: '{0}'", normPath);
    else {
        Log(this).info(
            "Pre-parse analysis on file: '{0}':\n - Top-level Definitions: {1} ({2} already "
            "parsed)\n - Files:         "
            "        {3} ({4} already parsed)",
            normPath,
            analysis.totalDefinitionCount,
            analysis.preparsedDefinitionCount,
            analysis.totalFileCount,
            analysis.preparsedFileCount);
    }

    bool            success         = true;
    auto            definitionCount = analysis.preparsedDefinitionCount;
    def::FileParser parser(normPath, fileStore, outlineDefinitions);
    while (true) {
        if (not analysis.failed) {
            const auto definitionsToParse = analysis.totalDefinitionCount - analysis.preparsedDefinitionCount;
            const auto percent =
                static_cast<uint8_t>((static_cast<float_s>(definitionCount) / definitionsToParse) * 100.f);
            Log(this).info("\r[{0}/{1} | {2}%] Parsing definitions...", definitionCount, definitionsToParse, percent);
        }

        auto entry = parser.nextDefinition();
        if (not parser.isOpen()) {
            Log(this).success("Parsing completed.");
            Log(this).info("Currently storing {0} definitions.", totalDefinitionCount());
            break;
        }

        if (not entry) {
            Log(this).error("Parsing aborted due to invalid definition.");
            success = false;
            continue;
        }

        success &= addDefinition(std::move(*entry));
        ++definitionCount;
    }

    estimators.dropUnusedEstimators();
    outlineDefinitions.clear();
    return success;
}

void DataStore::dump(const std::string& path, const bool prettify) const
{
    std::ofstream out(path);
    if (not out.is_open()) {
        Log(this).fatal("Failed to open file: '{0}' for writing.", path);
        return;
    }

    out << ":.\n"
        << "   - Chemgine Generated Definition File -\n\n   Version: 0.0.0\n   Sources:\n";
    for (const auto& f : fileStore.getHistory())
        out << "    > " << f.first << '\n';
    out << ".:\n\n";

    for (auto a = atoms.atomsBegin(); a != atoms.atomsEnd(); ++a)
        a->second->dumpDefinition(out, prettify);
    for (auto r = atoms.radicalsBegin(); r != atoms.radicalsEnd(); ++r)
        r->second->dumpDefinition(out, prettify);

    std::unordered_set<EstimatorId> printedEstimators;
    for (const auto& m : molecules)
        m.second->dumpDefinition(out, prettify, printedEstimators);
    for (const auto& r : reactions)
        r.second->dumpDefinition(out, prettify, printedEstimators);

    const auto dir = utils::extractDirName(path);
    for (const auto& l : labware) {
        l.second->dumpDefinition(out, prettify);
        l.second->dumpTextures(dir);
    }

    out.close();
}

void DataStore::clear()
{
    labware.clear();
    reactions.clear();
    molecules.clear();
    estimators.clear();
    atoms.clear();
    outlineDefinitions.clear();
    fileStore.clear();
}
