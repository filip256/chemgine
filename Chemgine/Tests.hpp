#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>
#include <functional>

#include "Logger.hpp"
#include "MolecularStructure.hpp"
#include "DataStore.hpp"
#include "Reactor.hpp"
#include "BaseLabwareComponent.hpp"
#include "Reactable.hpp"
#include "DumpContainer.hpp"
#include "Atmosphere.hpp"
#include "ForwardingContainer.hpp"

class MolecularStructureTest
{
private:
	bool passed = true;
	std::vector<MolecularStructure> setA, setB, setC, setD, setE, setF, setG;
	std::unordered_map<std::string, std::vector<uint8_t>> res;

public:
	void initialize()
	{
		res.emplace(std::make_pair("mapTo", std::vector<uint8_t>()));
		res.emplace(std::make_pair("==", std::vector<uint8_t>()));
		res.emplace(std::make_pair("maximal", std::vector<uint8_t>()));
		res.emplace(std::make_pair("addSub", std::vector<uint8_t>()));

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

		setA.emplace_back(std::move(MolecularStructure("O(CCC)CC")));
		setB.emplace_back(std::move(MolecularStructure("O(CC)(CCC)")));
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(true);

		setA.emplace_back(std::move(MolecularStructure("CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C")));
		setB.emplace_back(std::move(MolecularStructure("N(R)C14CC(CC=C1C2=C(OR)C=CC3=C2C(=C[N]3)C4)C(=O)N(R)R")));
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		setA.emplace_back(std::move(MolecularStructure("COC4=C2C1=C([N]C=C1CC3(CC(CC=C23)C(=O)N(C)C)NC(C)C)C=C4")));
		setB.emplace_back(std::move(MolecularStructure("N(R)C14CC(CC=C1C2=C(OR)C=CC3=C2C(=C[N]3)C4)C(=O)N(R)R")));
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		//-----//

		setC.emplace_back(std::move(MolecularStructure("CC(=O)OC")));
		setD.emplace_back(std::move(MolecularStructure("OCC")));
		res["maximal"].emplace_back(3);

		setC.emplace_back(std::move(MolecularStructure("C1CCCCC(O)CC1")));
		setD.emplace_back(std::move(MolecularStructure("CC(O)C")));
		res["maximal"].emplace_back(4);

		setC.emplace_back(std::move(MolecularStructure("CC(=O)OR")));
		setD.emplace_back(std::move(MolecularStructure("OCR")));
		res["maximal"].emplace_back(2);

		setC.emplace_back(std::move(MolecularStructure("C(=O)N(C)C")));
		setD.emplace_back(std::move(MolecularStructure("C1CCC1")));
		res["maximal"].emplace_back(1);

		setC.emplace_back(std::move(MolecularStructure("O(C)CC")));
		setD.emplace_back(std::move(MolecularStructure("O(CC)C")));
		res["maximal"].emplace_back(4);

		setC.emplace_back(std::move(MolecularStructure("CC2CCCC(C1CCCCC1)C2")));
		setD.emplace_back(std::move(MolecularStructure("CC1CCCCC1")));
		res["maximal"].emplace_back(7);

		//-----//

		setE.emplace_back(std::move(MolecularStructure("CC(=O)OC")));
		setF.emplace_back(std::move(MolecularStructure("OCCC")));
		res["addSub"].emplace_back(6);


		setE.emplace_back(std::move(MolecularStructure("CC(=O)OR")));
		setF.emplace_back(std::move(MolecularStructure("OCCC")));
		res["addSub"].emplace_back(6);

		setE.emplace_back(std::move(MolecularStructure("CCC(=O)O")));
		setF.emplace_back(std::move(MolecularStructure("CC(=O)OCC")));
		res["addSub"].emplace_back(7);

		setE.emplace_back(std::move(MolecularStructure("C(=O)O")));
		setF.emplace_back(std::move(MolecularStructure("CC(=O)OC(C)C")));
		res["addSub"].emplace_back(7);

		setE.emplace_back(std::move(MolecularStructure("C1CCCC1")));
		setF.emplace_back(std::move(MolecularStructure("C1CC(O)C1")));
		res["addSub"].emplace_back(6);

		setE.emplace_back(std::move(MolecularStructure("CC(=C)C")));
		setF.emplace_back(std::move(MolecularStructure("C1CCC1O")));
		res["addSub"].emplace_back(6);

		setG.emplace_back(std::move(MolecularStructure("C1CCC1")));
		setG.emplace_back(std::move(MolecularStructure("C1C(C)C(C)C1")));
		setG.emplace_back(std::move(MolecularStructure("C1C(O)C(C)C1")));
		setG.emplace_back(std::move(MolecularStructure("CC(=O)C")));
		setG.emplace_back(std::move(MolecularStructure("CC(=O)OC(=O)C")));
		setG.emplace_back(std::move(MolecularStructure("C1CC12CC2")));
		setG.emplace_back(std::move(MolecularStructure("C1C(O)CC12CC2")));
		setG.emplace_back(std::move(MolecularStructure("OC1(CC1)C")));
		setG.emplace_back(std::move(MolecularStructure("OC1C2CC12")));
		setG.emplace_back(std::move(MolecularStructure("CC2CCCC(C1CCCCC1)C2")));
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
				passed = false;
			}

			if ((setA[i] == setB[i]) != res["=="][i])
			{
				Logger::log("Test failed > MolecularStructure > == > #" + std::to_string(i)
					+ ": expected=" + std::to_string(res["=="][i]) + "\n"
					+ setA[i].print() + '\n' + setB[i].print(), LogType::BAD);
				passed = false;
			}
		}

		for (size_t i = 0; i < setC.size(); ++i)
		{
			if (setC[i].maximalMapTo(setD[i]).first.size() != res["maximal"][i])
			{
				Logger::log("Test failed > MolecularStructure > maximalMapTo > #" + std::to_string(i)
					+ ": expected=" + std::to_string(res["maximal"][i]) + "\n"
					+ setC[i].print() + '\n' + setD[i].print(), LogType::BAD);
				passed = false;
			}
		}

		for (size_t i = 0; i < setG.size(); ++i)
		{
			if (MolecularStructure(setG[i].serialize(), true) != setG[i])
			{
				Logger::log("Test failed > MolecularStructure > serialize/deserialize > #" + std::to_string(i)
					+ ": expected= true\n"
					+ setG[i].print(), LogType::BAD);
				passed = false;
			}
		}

		for (size_t i = 0; i < setG.size(); ++i)
		{
			if (MolecularStructure(setG[i].toSMILES()) != setG[i])
			{
				Logger::log("Test failed > MolecularStructure > SMILES > #" + std::to_string(i)
					+ ": expected= true\n"
					+ setG[i].print(), LogType::BAD);
				passed = false;
			}
		}

		for (size_t i = 0; i < setE.size(); ++i)
		{
			auto map = setF[i].maximalMapTo(setE[i]).first;
			const auto newMol = MolecularStructure::addSubstituents(setE[i], setF[i], map);
			if (newMol.componentCount() != res["addSub"][i])
			{
				Logger::log("Test failed > MolecularStructure > addSubstituents > #" + std::to_string(i)
					+ ": expected=" + std::to_string(res["addSub"][i]) + "\n"
					+ setE[i].print() + '\n' + setF[i].print(), LogType::BAD);
				passed = false;
			}
		}
	}

	bool hasPassed()
	{
		return passed;
	}
};

class EstimatorTest
{
	class ReferenceSet
	{
	private:
		const std::map<double, double> refData;

	public:
		ReferenceSet(std::initializer_list<std::pair<double, double>> initializer) noexcept :
			refData(initializer.begin(), initializer.end())
		{}

		ReferenceSet(const ReferenceSet&) = delete;
		ReferenceSet(ReferenceSet&&) = default;

		double testAE(const double input, const double actOutput)
		{
			if (refData.contains(input) == false)
			{
				Logger::log("Missing reference data for input:" + std::to_string(input), LogType::WARN);
				return 0;
			}

			const auto err = abs(refData.at(input) - actOutput);
			return err;
		}

		double testMAE(const std::function<double(double)>& estimator) const
		{
			Logger::enterContext();
			Logger::logCached("Input    |   Reference  Actual     Error", LogType::TABLE);
			double tErr = 0.0;
			for (const auto& p : refData)
			{	
				const auto act = estimator(p.first);
				const auto err = abs(p.second - act);
				tErr += err;

				Logger::logCached(std::to_string(p.first).substr(0, 8) + " |   " + std::to_string(p.second).substr(0, 8) + "   " + std::to_string(act).substr(0, 8) + "   " + std::to_string(err), LogType::TABLE);
			}
			const auto mae = tErr / refData.size();

			Logger::logCached("---------+------------------------------------", LogType::TABLE);
			Logger::logCached("MAE:     |   " + std::to_string(mae).substr(0, 16), LogType::TABLE);
			Logger::exitContext();
			Logger::logCached("", LogType::TABLE);

			return mae;
		}
	};

private:
	bool passed = true;

	const ReferenceSet waterBpRef = ReferenceSet({ {1400, 118.1}, {1000, 107.8}, {900, 104.8}, {800, 101.4}, {760, 100}, {600, 93.3}, {500, 89.0}, {400, 83.3}, {300, 75.6}, {200, 66.5}, {100, 51.9}, {1, -16.9} });
	const double waterDensityThreshold = 0.01;
	const ReferenceSet waterDensityRef = ReferenceSet({ {0.0, 0.99989}, {1.0, 0.99992}, {3.0, 0.99996}, {4.0, 0.99995}, {5.0, 0.99993}, {10.0, 0.99965}, {30.0, 0.99567}, {80.0, 0.97176}, {99.0, 0.95909} });
	const double waterBpThreshold = 2.0;

public:

	void runTests()
	{
		const Molecule water = Molecule("O");
		auto mae = waterBpRef.testMAE([&water](double input) {return water.getBoilingPointAt(input).asStd(); });
		if (mae > waterBpThreshold)
		{
			Logger::log("Test failed > Estimator > waterBp: MAE=" + std::to_string(mae), LogType::BAD);
			Logger::printCache();
			passed = false;
		}
		Logger::clearCache();

		mae = waterDensityRef.testMAE([&water](double input) {return water.getDensityAt(input, 760.0).asStd(); });
		if (mae > waterDensityThreshold)
		{
			Logger::log("Test failed > Estimator > waterDensity: MAE=" + std::to_string(mae), LogType::BAD);
			Logger::printCache();
			passed = false;
		}
		Logger::clearCache();
	}

	bool hasPassed()
	{
		return passed;
	}
};

class ReactorTest
{
private:
	bool passed = true;
	DumpContainer* dumpA = nullptr;
	Atmosphere* atmosphere = nullptr;
	Reactor* reactorA = nullptr;
	Reactor* reactorB = nullptr;
	Reactor* reactorC = nullptr;
	Reactor* reactorD = nullptr;
	Reactor* reactorE = nullptr;
	Reactor* reactorF = nullptr;
	Reactor* reactorG = nullptr;
	SingleLayerMixture<LayerType::GASEOUS>* gasMixtureA = nullptr;
	ForwardingContainer* forwardA = nullptr;
	const Amount<Unit::SECOND> tickTimespan = 1.0_s;

	const double waterTemperatureThreshold = 0.1;
	const double overflowLossThreshold = 0.0000001;
	const double determinismEqualityThreshold = std::numeric_limits<double>::epsilon();

	void runConservationOfMassTest()
	{
		const auto massBefore = reactorA->getTotalMass() + atmosphere->getTotalMass() + dumpA->getTotalMass();
		for (size_t i = 0; i < 32; ++i)
		{
			reactorA->tick(tickTimespan);
			atmosphere->tick(tickTimespan);
			const auto massAfter = reactorA->getTotalMass() + atmosphere->getTotalMass() + dumpA->getTotalMass();

			if (std::abs((massAfter - massBefore).asStd()) > 1e-5)
			{
				Logger::log("Test failed > Reactor > mass conservation: expected=" + massBefore.toString() + "   actual=" + massAfter.toString(), LogType::BAD);
				passed = false;
				break;
			}
		}
	}
	void runTemperatureTest()
	{
		Logger::enterContext();
		Logger::logCached("Input    |   Reference  Actual     Error", LogType::TABLE);

		const auto& layer = reactorB->getLayer(LayerType::POLAR);
		const auto moles = layer.getMoles();
		Amount<Unit::JOULE> addedEnergy = 0.0;
		double tErr = 0.0;
		std::vector<std::pair<Amount<Unit::JOULE>, Amount<Unit::CELSIUS>>> batches { {0.0, 1.0 }, { 7.5, 1.1 }, { 30.19, 1.5 }, { 264.19, 5.0 }, { 754.84, 15.0 }, { 6408.59, 99.9 }, {-7465.310 , 1.0}};
		for (size_t i = 0; i < batches.size(); ++i)
		{
			const auto correctedEnergy = batches[i].first * moles.asStd();
			reactorB->add(correctedEnergy);
			reactorB->tick(tickTimespan);
			addedEnergy += correctedEnergy;
			const auto act = layer.getTemperature();
			const auto err = abs((act - batches[i].second).asStd());
			tErr += err;

			Logger::logCached(addedEnergy.toString(7) + " |   " + batches[i].second.toString(7) + "   " + act.toString(7) + "   " + std::to_string(err), LogType::TABLE);
		}
		const auto mae = tErr / batches.size();

		Logger::logCached("---------+------------------------------------", LogType::TABLE);
		Logger::logCached("MAE:     |   " + std::to_string(mae).substr(0, 16), LogType::TABLE);
		Logger::exitContext();
		Logger::logCached("", LogType::TABLE);

		if (mae > waterTemperatureThreshold)
		{
			Logger::log("Test failed > Reactor > waterTemp: MAE=" + std::to_string(mae), LogType::BAD);
			Logger::printCache();
			passed = false;
		}
		Logger::clearCache();
	}
	void runVolumetricTest()
	{
		if (std::abs((reactorC->getTotalVolume() - reactorC->getMaxVolume()).asStd()) > overflowLossThreshold)
		{
			Logger::log("Test failed > Reactor > volumetrics > total_volume: expected=" + reactorC->getMaxVolume().toString() + "   actual=" + reactorC->getTotalVolume().toString(), LogType::BAD);
			passed = false;
		}

		reactorC->add(Molecule("O"), 700.0);
		const auto atmBefore = atmosphere->getTotalVolume();
		const auto reactorBefore = reactorC->getTotalVolume();
		reactorC->tick(tickTimespan);
		const auto atmAfter = atmosphere->getTotalVolume();
		const auto reactorAfter = reactorC->getTotalVolume();

		auto loss = abs((atmBefore + reactorBefore - atmAfter - reactorAfter).asStd());
		if(loss > overflowLossThreshold)
		{
			Logger::log("Test failed > Reactor > volumetrics > overflow: Total volume loss=" + std::to_string(loss), LogType::BAD);
			passed = false;
		}

		loss = abs((reactorAfter - reactorC->getMaxVolume()).asStd());
		if(loss > overflowLossThreshold)
		{
			Logger::log("Test failed > Reactor > volumetrics > overflow: Source volume loss=" + std::to_string(loss), LogType::BAD);
			passed = false;
		}
	}
	void runIncompatibleForwardingTest()
	{
		const auto water = Reactant(Molecule("O"), LayerType::POLAR, 1.0_mol);
		const auto oxygen = Reactant(Molecule("O=O"), LayerType::GASEOUS, 1.0_mol);

		gasMixtureA->add(water);
		if (reactorF->getAmountOf(water) != water.amount)
		{
			Logger::log("Test failed > SingleLayerMixture > incompatible forwarding: Incompatible reactant was not forwarded.", LogType::BAD);
			passed = false;
		}

		const auto molesBefore = reactorF->getTotalMoles();
		gasMixtureA->add(oxygen);
		if (reactorF->getTotalMoles() != molesBefore)
		{
			Logger::log("Test failed > SingleLayerMixture > incompatible forwarding: Compatible reactant was forwarded.", LogType::BAD);
			passed = false;
		}
	}
	void runImplicitForwardingTest()
	{
		const auto water = Reactant(Molecule("O"), LayerType::POLAR, 1.0_mol);
		const auto oxygen = Reactant(Molecule("O=O"), LayerType::GASEOUS, 1.0_mol);

		forwardA->add(water);
		if (reactorG->getAmountOf(water) != water.amount)
		{
			Logger::log("Test failed > ForwardContainer: Reactant was not forwarded correctly.", LogType::BAD);
			passed = false;
		}

		const auto molesBefore = reactorG->getTotalMoles();
		forwardA->add(oxygen);
		if (reactorG->getTotalMoles() != molesBefore)
		{
			Logger::log("Test failed > ForwardContainer: Reactant was not forwarded correctly.", LogType::BAD);
			passed = false;
		}
	}
	void runDeterminismTest()
	{
		bool testPassed = true;
		auto initialReactor = reactorD->makeCopy();
		auto copyReactor = reactorD->makeCopy();

		reactorD->add(10.0_J);
		copyReactor.add(10.0_J);
		reactorD->tick(tickTimespan);
		copyReactor.tick(tickTimespan);

		if (reactorD->isSame(initialReactor))
		{
			Logger::log("Test failed > Reactor > determinism: given reactors were already stable", LogType::BAD);
			testPassed = false;
			return;
		}

		for (size_t i = 0; i < 127; ++i)
		{
			double err = abs((reactorD->getPressure() - copyReactor.getPressure()).asStd());
			if (err > determinismEqualityThreshold)
			{
				Logger::log("Test failed > Reactor > determinism: reactors have different states: pressure, loss=" + std::to_string(err), LogType::BAD);
				testPassed = false;
			}

			err = abs((reactorD->getTotalMoles() - copyReactor.getTotalMoles()).asStd());
			if (err > determinismEqualityThreshold)
			{
				Logger::log("Test failed > Reactor > determinism: reactors have different states: total moles, loss=" + std::to_string(err), LogType::BAD);
				testPassed = false;
			}

			err = abs((reactorD->getTotalMass() - copyReactor.getTotalMass()).asStd());
			if (err > determinismEqualityThreshold)
			{
				Logger::log("Test failed > Reactor > determinism: reactors have different states: total mass, loss=" + std::to_string(err), LogType::BAD);
				testPassed = false;
			}

			err = abs((reactorD->getTotalVolume() - copyReactor.getTotalVolume()).asStd());
			if (err > determinismEqualityThreshold)
			{
				Logger::log("Test failed > Reactor > determinism: reactors have different states: total volume, loss=" + std::to_string(err), LogType::BAD);
				testPassed = false;
			}

			if (reactorD->hasSameContent(copyReactor, determinismEqualityThreshold) == false)
			{
				Logger::log("Test failed > Reactor > determinism: reactors have different contents", LogType::BAD);
				testPassed = false;
			}

			if (reactorD->hasSameLayers(copyReactor, determinismEqualityThreshold) == false)
			{
				Logger::log("Test failed > Reactor > determinism: reactors have different layers", LogType::BAD);
				testPassed = false;
			}

			if (testPassed == false)
			{
				passed = false;
				//break;
			}

			reactorD->add(10.0_J);
			copyReactor.add(10.0_J);
			reactorD->tick(tickTimespan);
			copyReactor.tick(tickTimespan);
		}
	}
	void runAggregationChangeTest()
	{
		Logger::enterContext();
		Logger::logCached("Energy  |   SourceTemp   DestinationTemp  Source Nucleator Amount", LogType::TABLE);

		const auto& source = reactorE->getLayer(LayerType::POLAR);
		const auto& destination = reactorE->getLayer(LayerType::GASEOUS);

		bool testPassed = true;
		uint8_t testPhase = 0;
		const auto sourceMaxTemp = source.getMaxAllowedTemperature();
		const auto nucleator = source.getHighNucleator();
		auto pastNucleatorAmount = reactorE->getAmountOf(nucleator);
		auto pastSourceTemp = source.getTemperature();
		auto pastDestinationTemp = destination.getTemperature();

		Amount<Unit::JOULE> energyStep = 6000.0;
		Logger::logCached("0kJ     |   " +
			source.getTemperature().toString(8) + "    " +
			destination.getTemperature().toString(8) + "        " +
			reactorE->getAmountOf(nucleator).toString(8),
			LogType::TABLE);

		for (size_t i = 0; i < 64; ++i)
		{
			reactorE->add(Amount<Unit::JOULE>(energyStep));
			reactorE->tick(tickTimespan);
			Logger::logCached(std::to_string(energyStep.asKilo() * (i + 1)).substr(0, 5) + "kJ |   " +
				source.getTemperature().toString(8) + "    " +
				destination.getTemperature().toString(8) + "        " +
				reactorE->getAmountOf(nucleator).toString(8),
				LogType::TABLE);

			if (testPhase == 0) // heating up source to tp
			{
				const auto sTemp = source.getTemperature();
				if (sTemp == sourceMaxTemp)
				{
					++testPhase;
					pastSourceTemp = sTemp;
				}
				else if (sTemp >= sourceMaxTemp)
				{
					Logger::log("Test failed > Reactor > aggregation change > max source temp exceeded, T=" + sTemp.toString(), LogType::BAD);
					testPassed = false;
					break;
				}

				const auto dTemp = destination.getTemperature();;
				if (dTemp != pastDestinationTemp)
				{
					Logger::log("Test failed > Reactor > aggregation change > destination temperature changed in phase 0", LogType::BAD);
					testPassed = false;
					break;
				}
				pastDestinationTemp = dTemp;
			}
			else if (testPhase == 1) // heating up dest to tp
			{
				const auto dTemp = destination.getTemperature();
				if (dTemp == sourceMaxTemp)
				{
					++testPhase;
					pastDestinationTemp = dTemp;
				}
				else if (dTemp >= sourceMaxTemp)
				{
					Logger::log("Test failed > Reactor > aggregation change > max destination temp exceeded, T=" + dTemp.toString(), LogType::BAD);
					testPassed = false;
					break;
				}

				const auto sTemp = source.getTemperature();;
				if (sTemp != pastSourceTemp)
				{
					Logger::log("Test failed > Reactor > aggregation change > source temperature changed in phase 1", LogType::BAD);
					testPassed = false;
					break;
				}
				pastSourceTemp = sTemp;
			}
			else if (testPhase == 2) // transfering all the nucleator
			{
				const auto nMoles = reactorE->getAmountOf(nucleator);
				if (nMoles >= pastNucleatorAmount)
				{
					Logger::log("Test failed > Reactor > aggregation change > nucleator did not transfer in phase 2", LogType::BAD);
					testPassed = false;
					break;
				}
				pastNucleatorAmount = nMoles;

				const auto dTemp = destination.getTemperature();;
				if (dTemp != pastDestinationTemp)
				{
					Logger::log("Test failed > Reactor > aggregation change > destination temperature changed in phase 2", LogType::BAD);
					testPassed = false;
					break;
				}
				pastDestinationTemp = dTemp;

				if (nMoles == 0.0)
					++testPhase;
			}
			else if (testPhase == 3) // full heat convertsion
			{
				const auto dTemp = destination.getTemperature();
				if (dTemp <= pastDestinationTemp)
				{
					Logger::log("Test failed > Reactor > aggregation change > destination temperature did not increase in phase 3", LogType::BAD);
					testPassed = false;
					break;
				}
				pastDestinationTemp = dTemp;
				if (source.getTemperature().isInfinity() == false)
				{
					Logger::log("Test failed > Reactor > aggregation change > empty layer temperature was not infinity", LogType::BAD);
					testPassed = false;
					break;
				}
			}
		}

		Logger::logCached("--------+--------------------------------------------", LogType::TABLE);

		if(testPassed && testPhase != 3)
		{
			Logger::log("Test failed > Reactor > aggregation change > not all test phases were reached, phase=" + std::to_string(testPhase), LogType::BAD);
			testPassed = false;
		}

		Logger::exitContext();
		Logger::logCached("", LogType::TABLE);

		if (testPassed == false)
		{
			Logger::printCache();
			passed = false;
		}

		Logger::clearCache();
	}

public:
	void initialize()
	{
		dumpA = new DumpContainer();
		atmosphere = new Atmosphere(
			1.0_C, 760.0_torr,
			{ { Molecule("N#N"), 78.084_mol }, { Molecule("O=O"), 20.946_mol } },
			1000.0_L, *dumpA, 1);

		reactorA = new Reactor(*atmosphere, 1.0_L, 1);
		reactorA->add(Molecule("HH"), 2.0_mol);
		reactorA->add(Molecule("CC=C"), 2.0_mol);
		reactorA->add(Molecule("CC(=O)OCC"), 2.0_mol);
		reactorA->add(Molecule("O"), 3.0_mol);

		reactorB = new Reactor(*atmosphere, 1.0_L, 1);
		reactorB->setTickMode(reactorB->getTickMode() - TickMode::ENABLE_CONDUCTION);
		reactorB->add(Molecule("O"), 3.0_mol);

		reactorC = new Reactor(*atmosphere, 20.0_L, 1);

		reactorD = new Reactor(*atmosphere, 5.0_L, 1);
		reactorD->setTickMode(reactorD->getTickMode());
		reactorD->add(Molecule("CC(=O)O"), 2.0_mol);
		reactorD->add(Molecule("OCC"), 3.0_mol);

		reactorE = new Reactor(*atmosphere, 0.1_L, 1);
		reactorE->setTickMode(reactorE->getTickMode() - TickMode::ENABLE_CONDUCTION);
		reactorE->add(Molecule("O"), 5.4_mol);
		
		reactorF = new Reactor(*atmosphere, 1.0_L, 1);
		gasMixtureA = new SingleLayerMixture<LayerType::GASEOUS>(
			1.0_C, 760.0_torr, { { Molecule("N#N"), 78.084_mol } }, 0.5_L, *dumpA, 1);
		gasMixtureA->setIncompatibilityTarget(LayerType::POLAR, *reactorF);

		reactorG = new Reactor(*atmosphere, 1.0_L, 1);
		forwardA = new ForwardingContainer(
			{
				{ [](const Reactant& reactant) -> bool { return reactant.molecule.getMolarMass() < 20; }, Ref<BaseContainer>(*reactorG)}
			},
			Ref<BaseContainer>(*dumpA)
		);
	}

	void runTests()
	{
		runConservationOfMassTest();
		runTemperatureTest();
		runVolumetricTest();
		runIncompatibleForwardingTest();
		runImplicitForwardingTest();
		runDeterminismTest();
		runAggregationChangeTest();
	}

	bool hasPassed()
	{
		return passed;
	}

	~ReactorTest()
	{
		if (reactorA != nullptr)
			delete reactorA;
		if (reactorB != nullptr)
			delete reactorB;
		if (reactorC != nullptr)
			delete reactorC;
		if (reactorD != nullptr)
			delete reactorD;
		if (reactorE != nullptr)
			delete reactorE;
		if (gasMixtureA != nullptr)
			delete gasMixtureA;
		if (reactorF != nullptr)
			delete reactorF;
		if (forwardA != nullptr)
			delete forwardA;
		if (reactorG != nullptr)
			delete reactorG;
		if (atmosphere != nullptr)
			delete atmosphere;
		if (dumpA != nullptr)
			delete dumpA;
	}
};


class TestManager
{
private:
	DataStore store;
	MolecularStructureTest molecularStructureTest;
	ReactorTest reactorTest;
	EstimatorTest estimatorTest;

public:
	TestManager()
	{
		Logger::enterContext();
		const auto begin = std::chrono::steady_clock::now();
		BaseComponent::setDataStore(store);
		Reactable::setDataStore(store);
		Reactor::setDataStore(store);
		Molecule::setDataStore(store);
		BaseLabwareComponent::setDataStore(store);
		store.loadAtomsData("Data/AtomData.csv")
			.loadEstimatorsData("")
			.loadMoleculesData("Data/MoleculeData.csv")
			.loadGenericMoleculesData("Data/GenericMoleculeData.csv")
			.loadReactionsData("Data/ReactionData.csv")
			.loadLabwareData("Data/LabwareData.csv");

		std::cout << '\n' << store.reactions.getNetwork().print() << '\n';

		molecularStructureTest.initialize();
		reactorTest.initialize();
		const auto end = std::chrono::steady_clock::now();
		Logger::log("Test initialization completed in " +
			std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0) + "s.");
	}

	~TestManager()
	{
		Logger::exitContext();
	}

	void runAll()
	{
		const auto begin = std::chrono::steady_clock::now();
		molecularStructureTest.runTests();
		estimatorTest.runTests();
		reactorTest.runTests();
		const auto end = std::chrono::steady_clock::now();

		Logger::log("Test execution completed in " +
			std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0) + "s.");

		if (molecularStructureTest.hasPassed())
			Logger::log("All MolecularStructure tests passed.", LogType::GOOD);
		if (estimatorTest.hasPassed())
			Logger::log("All Estimator tests passed.", LogType::GOOD);
		if (reactorTest.hasPassed())
			Logger::log("All Reactor tests passed.", LogType::GOOD);
	}

	void runPersist()
	{
		const auto begin = std::chrono::steady_clock::now();
		store.reactions.generateTotalSpan();
		store.saveGenericMoleculesData("Out/genericmolecules.out.csv")
			.saveMoleculesData("Out/molecules.out.csv");
		const auto end = std::chrono::steady_clock::now();

		Logger::log("Total span dump completed in " +
			std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0) + "s.");
	}
};