#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>

#include "Logger.hpp"
#include "MolecularStructure.hpp"
#include "DataStore.hpp"

class MolecularStructureTest
{
private:
	std::vector<MolecularStructure> setA, setB;
	std::unordered_map<std::string, std::vector<uint8_t>> res;

public:
	void initialize()
	{
		res.emplace(std::make_pair("mapTo", std::vector<uint8_t>()));
		res.emplace(std::make_pair("==", std::vector<uint8_t>()));

		setA.emplace_back(std::move(MolecularStructure("CN(C)C(=O)C1=CC=CC=C1")));
		setB.emplace_back(std::move(MolecularStructure("C1=CC=CC=C1R")));
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		setA.emplace_back(std::move(MolecularStructure("CC(=O)OC")));
		setB.emplace_back(std::move(MolecularStructure("RC(=O)OR")));
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		setA.emplace_back(std::move(MolecularStructure("CC(=O)OC")));
		setB.emplace_back(std::move(MolecularStructure("RC(=O)O")));
		res["mapTo"].emplace_back(false);
		res["=="].emplace_back(false);

		setA.emplace_back(std::move(MolecularStructure("CC(=O)N(C)C")));
		setB.emplace_back(std::move(MolecularStructure("RC(=O)N(R)R")));
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		setA.emplace_back(std::move(MolecularStructure("O(C)(CC)")));
		setB.emplace_back(std::move(MolecularStructure("O(C)(C)")));
		res["mapTo"].emplace_back(false);
		res["=="].emplace_back(false);

		setA.emplace_back(std::move(MolecularStructure("C1CC1")));
		setB.emplace_back(std::move(MolecularStructure("C1CC1")));
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(true);

		setA.emplace_back(std::move(MolecularStructure("C1CCC1")));
		setB.emplace_back(std::move(MolecularStructure("C1CC1")));
		res["mapTo"].emplace_back(false);
		res["=="].emplace_back(false);

		setA.emplace_back(std::move(MolecularStructure("C1C(OC)CC1")));
		setB.emplace_back(std::move(MolecularStructure("C1CC(OR)C1")));
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		setA.emplace_back(std::move(MolecularStructure("CC(O)C")));
		setB.emplace_back(std::move(MolecularStructure("OR")));
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		setA.emplace_back(std::move(MolecularStructure("CC1CCCC2CCCCC12")));
		setB.emplace_back(std::move(MolecularStructure("CC1CCCC2CCCCC12")));
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(true);

		setA.emplace_back(std::move(MolecularStructure("CC1=CC2=C(NC=C2)C=C1")));
		setB.emplace_back(std::move(MolecularStructure("RC1=CC2=C(NC=C2)C=C1")));
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		setA.emplace_back(std::move(MolecularStructure("C1CC2=C1C=C2")));
		setB.emplace_back(std::move(MolecularStructure("RC1=C(R)CC1")));
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		setA.emplace_back(std::move(MolecularStructure("C(C)(C)OC")));
		setB.emplace_back(std::move(MolecularStructure("O(R)R")));
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		// visitedB bug
		setA.emplace_back(std::move(MolecularStructure("O(CCC)CC")));
		setB.emplace_back(std::move(MolecularStructure("O(CC)(CCC)")));
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(true);

		setA.emplace_back(std::move(MolecularStructure("CCNC12CC(CC=C1C1=C(OC)C=CC3=C1C(C2)=CN3)C(=O)N(C)C")));
		setB.emplace_back(std::move(MolecularStructure("RNC12CC(CC=C1C1=C(OR)C=CC3=C1C(C2)=CN3)C(=O)N(R)R")));
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		setA.emplace_back(std::move(MolecularStructure("COC1=C2C3=C(NC=C3CC3(CC(CC=C23)C(=O)N(C)C)NC(C)C)C=C1")));
		setB.emplace_back(std::move(MolecularStructure("RNC12CC(CC=C1C1=C(OR)C=CC3=C1C(C2)=CN3)C(=O)N(R)R")));
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);
	}

	void runTests()
	{
		for (size_t i = 0; i < setA.size(); ++i)
		{
			if (setA[i].mapTo(setB[i], true).size() > 0 != res["mapTo"][i])
			{
				Logger::log("Test failed > MolecularStructure > mapTo > #" + std::to_string(i)
					+ ": expected=" + std::to_string(res["mapTo"][i]) + "\n"
					+ setA[i].print() + '\n' + setB[i].print(), LogType::BAD);
			}

			if ((setA[i] == setB[i]) != res["=="][i])
			{
				Logger::log("Test failed > MolecularStructure > == > #" + std::to_string(i)
					+ ": expected=" + std::to_string(res["=="][i]) + "\n"
					+ setA[i].print() + '\n' + setB[i].print(), LogType::BAD);
			}
		}
	}
};

class TestManager
{
private:
	DataStore r;
	MolecularStructureTest molecularStructureTest;

public:
	TestManager()
	{
		const auto begin = std::chrono::steady_clock::now();
		BaseComponent::setDataStore(r);
		r.loadAtomsData("Data/AtomData.csv");
		r.loadFunctionalGroupsData("Data/FunctionalGroupData.csv");
		r.loadBackbonesData("Data/BackboneData.csv");
		r.loadMoleculesData("Data/OrganicMoleculeData.csv");
		r.loadReactionsData("Data/ReactionData.csv");

		molecularStructureTest.initialize();
		const auto end = std::chrono::steady_clock::now();
		Logger::log("Test initialization completed in " +
			std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0) + "s.");
	}

	void runAll()
	{
		//std::cout << MolecularStructure("CC(=O)OC").print() << '\n';
		//std::cout << MolecularStructure("CONC").print() << '\n';
		const auto begin = std::chrono::steady_clock::now();
		molecularStructureTest.runTests();
		const auto end = std::chrono::steady_clock::now();
		Logger::log("Test execution completed in " +
			std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0) + "s.");
	}
};