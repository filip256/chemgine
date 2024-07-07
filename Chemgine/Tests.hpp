#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>
#include <functional>

#include "Log.hpp"
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
	std::vector<MolecularStructure> setA, setB, setC, setD, setE, setF, setG, setM;
	std::unordered_map<std::string, std::vector<uint8_t>> res;
	std::unordered_map<std::string, std::vector<float>> resFloat;

	const float massThreshold = 1.0f;

public:
	void initialize()
	{
		res.emplace(std::make_pair("mapTo", std::vector<uint8_t>()));
		res.emplace(std::make_pair("==", std::vector<uint8_t>()));
		res.emplace(std::make_pair("maximal", std::vector<uint8_t>()));
		res.emplace(std::make_pair("addSub", std::vector<uint8_t>()));
		resFloat.emplace(std::make_pair("mass", std::vector<float>()));

		setA.emplace_back("CN(C)C(=O)C1=CC=CC=C1");
		setB.emplace_back("C1=CC=CC=C1R");
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		setA.emplace_back("CC(=O)OC");
		setB.emplace_back("RC(=O)OR");
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		setA.emplace_back("CC(=O)OC");
		setB.emplace_back("RC(=O)O");
		res["mapTo"].emplace_back(false);
		res["=="].emplace_back(false);

		setA.emplace_back("CC(=O)N(C)C");
		setB.emplace_back("RC(=O)N(R)R");
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		setA.emplace_back("O(C)(CC)");
		setB.emplace_back("O(C)(C)");
		res["mapTo"].emplace_back(false);
		res["=="].emplace_back(false);

		setA.emplace_back("C1CC1");
		setB.emplace_back("C1CC1");
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(true);

		setA.emplace_back("C1CCC1");
		setB.emplace_back("C1CC1");
		res["mapTo"].emplace_back(false);
		res["=="].emplace_back(false);

		setA.emplace_back("C1C(OC)CC1");
		setB.emplace_back("C1CC(OR)C1");
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		setA.emplace_back("CC(O)C");
		setB.emplace_back("OR");
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		setA.emplace_back("CC1CCCC2CCCCC12");
		setB.emplace_back("CC1CCCC2CCCCC12");
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(true);

		setA.emplace_back("CC1=CC2=C(NC=C2)C=C1");
		setB.emplace_back("RC1=CC2=C(NC=C2)C=C1");
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		setA.emplace_back("C1CC2=C1C=C2");
		setB.emplace_back("RC1=C(R)CC1");
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		setA.emplace_back("C(C)(C)OC");
		setB.emplace_back("O(R)R");
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		setA.emplace_back("O(CCC)CC");
		setB.emplace_back("O(CC)(CCC)");
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(true);

		setA.emplace_back("CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C");
		setB.emplace_back("N(R)C14CC(CC=C1C2=C(OR)C=CC3=C2C(=C[N]3)C4)C(=O)N(R)R");
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		setA.emplace_back("COC4=C2C1=C([N]C=C1CC3(CC(CC=C23)C(=O)N(C)C)NC(C)C)C=C4");
		setB.emplace_back("N(R)C14CC(CC=C1C2=C(OR)C=CC3=C2C(=C[N]3)C4)C(=O)N(R)R");
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		setA.emplace_back("[Cl]");
		setB.emplace_back("X");
		res["mapTo"].emplace_back(true);
		res["=="].emplace_back(false);

		//-----//

		setC.emplace_back("CC(=O)OC");
		setD.emplace_back("OCC");
		res["maximal"].emplace_back(3);

		setC.emplace_back("C1CCCCC(O)CC1");
		setD.emplace_back("CC(O)C");
		res["maximal"].emplace_back(4);

		setC.emplace_back("CC(=O)OR");
		setD.emplace_back("OCR");
		res["maximal"].emplace_back(2);

		setC.emplace_back("C(=O)N(C)C");
		setD.emplace_back("C1CCC1");
		res["maximal"].emplace_back(1);

		setC.emplace_back("O(C)CC");
		setD.emplace_back("O(CC)C");
		res["maximal"].emplace_back(4);

		setC.emplace_back("CC2CCCC(C1CCCCC1)C2");
		setD.emplace_back("CC1CCCCC1");
		res["maximal"].emplace_back(7);

		//-----//

		setE.emplace_back("CC(=O)OC");
		setF.emplace_back("OCCC");
		res["addSub"].emplace_back(6);


		setE.emplace_back("CC(=O)OR");
		setF.emplace_back("OCCC");
		res["addSub"].emplace_back(6);

		setE.emplace_back("CCC(=O)O");
		setF.emplace_back("CC(=O)OCC");
		res["addSub"].emplace_back(7);

		setE.emplace_back("C(=O)O");
		setF.emplace_back("CC(=O)OC(C)C");
		res["addSub"].emplace_back(7);

		setE.emplace_back("C1CCCC1");
		setF.emplace_back("C1CC(O)C1");
		res["addSub"].emplace_back(6);

		setE.emplace_back("CC(=C)C");
		setF.emplace_back("C1CCC1O");
		res["addSub"].emplace_back(6);

		setG.emplace_back("C1CCC1");
		setG.emplace_back("C1C(C)C(C)C1");
		setG.emplace_back("C1C(O)C(C)C1");
		setG.emplace_back("CC(=O)C");
		setG.emplace_back("CC(=O)OC(=O)C");
		setG.emplace_back("C1CC12CC2");
		setG.emplace_back("C1C(O)CC12CC2");
		setG.emplace_back("OC1(CC1)C");
		//setG.emplace_back("OC1C2CC12");
		//setG.emplace_back("CC2CCCC(C1CCCCC1)C2");

		//-----//

		setM.emplace_back("CN(C)C(=O)C1=CC=CC=C1");
		resFloat["mass"].emplace_back(149.084f);

		setM.emplace_back("CC(=O)OC");
		resFloat["mass"].emplace_back(74.079f);

		setM.emplace_back("C1CCCC1");
		resFloat["mass"].emplace_back(70.1f);

		setM.emplace_back("CN1CC(C=C2C1CC3=CNC4=CC=CC2=C34)C(=O)O");
		resFloat["mass"].emplace_back(268.121f);

		setM.emplace_back("[Mg](O)O");
		resFloat["mass"].emplace_back(58.319f);
	}

	void runTests()
	{
		for (size_t i = 0; i < setA.size(); ++i)
		{
			if (setA[i].mapTo(setB[i], true).size() > 0 != res["mapTo"][i])
			{
				Log(this).error("Test failed > MolecularStructure > mapTo > #{0}: expected={1}\n{2}\n{3}",
					i, res["mapTo"][i], setA[i].print(), setB[i].print());
				passed = false;
			}

			if ((setA[i] == setB[i]) != res["=="][i])
			{
				Log(this).error("Test failed > MolecularStructure > == > #{0}: expected={1}\n{2}\n{3}",
					i, res["=="][i], setA[i].print(), setB[i].print());
				passed = false;
			}
		}

		for (size_t i = 0; i < setC.size(); ++i)
		{
			if (setC[i].maximalMapTo(setD[i]).first.size() != res["maximal"][i])
			{
				Log(this).error("Test failed > MolecularStructure > maximaMapTo > #{0}: expected={1}\n{2}\n{3}",
					i, res["maximal"][i], setC[i].print(), setD[i].print());
				passed = false;
			}
		}

		for (size_t i = 0; i < setG.size(); ++i)
		{
			if (MolecularStructure(setG[i].serialize(), true) != setG[i])
			{
				Log(this).error("Test failed > MolecularStructure > serialize/deserialize > #{0} : expected= true\n{1}",
					i, setG[i].print());
				passed = false;
			}
		}

		for (size_t i = 0; i < setG.size(); ++i)
		{
			if (MolecularStructure(setG[i].toSMILES()) != setG[i])
			{
				Log(this).error("Test failed > MolecularStructure > SMILES > #{0}: expected= true\n{1}",
					setG[i].print(), i, setG[i].print());
				passed = false;
			}
		}

		for (size_t i = 0; i < setE.size(); ++i)
		{
			auto map = setF[i].maximalMapTo(setE[i]).first;
			const auto newMol = MolecularStructure::addSubstituents(setE[i], setF[i], map);
			if (newMol.getNonVirtualAtomCount() != res["addSub"][i])
			{
				Log(this).error("Test failed > MolecularStructure > addSubstituents > #{0}: expected={1}\n{2}\n{3}",
					i, res["addSub"][i], setA[i].print(), setB[i].print());
				passed = false;
			}
		}

		for (size_t i = 0; i < setM.size(); ++i)
		{
			const auto mass = setM[i].getMolarMass().asStd();
			if (std::abs(mass - resFloat["mass"][i]) > massThreshold)
			{
				Log(this).error("Test failed > MolecularStructure > molarMass > #{0}: expected={1}\nbut got={2}",
					i, resFloat["mass"][i], mass);
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
				Log(this).warn("Missing reference data for input: {0}.", input);
				return 0;
			}

			const auto err = abs(refData.at(input) - actOutput);
			return err;
		}

		double testMAE(const std::function<double(double)>& estimator) const
		{
			Log<>::nest();
			Log(this).cache("Input    |   Reference  Actual     Error");
			double tErr = 0.0;
			for (const auto& p : refData)
			{	
				const auto act = estimator(p.first);
				const auto err = abs(p.second - act);
				tErr += err;

				Log(this).cache(std::to_string(p.first).substr(0, 8) + " |   " + std::to_string(p.second).substr(0, 8) + "   " + std::to_string(act).substr(0, 8) + "   " + std::to_string(err));
			}
			const auto mae = tErr / refData.size();

			Log(this).cache("---------+------------------------------------");
			Log(this).cache("MAE:     |   " + std::to_string(mae).substr(0, 16));
			Log<>::unnest();
			Log(this).cache("");

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
			Log(this).error("Test failed > Estimator > waterBp: MAE={0}.", mae);
			Log(this).printCache();
			passed = false;
		}
		Log(this).clearCache();

		mae = waterDensityRef.testMAE([&water](double input) {return water.getDensityAt(input, 760.0).asStd(); });
		if (mae > waterDensityThreshold)
		{
			Log(this).error("Test failed > Estimator > waterBp: MAE={0}.", mae);
			Log(this).printCache();
			passed = false;
		}
		Log(this).clearCache();
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
				Log(this).error("Test failed > Reactor > mass conservation: expected={0}   actual={1}",
					massBefore.toString(), massAfter.toString());
				passed = false;
				break;
			}
		}
	}
	void runTemperatureTest()
	{
		Log<>::nest();
		Log(this).cache("Input    |   Reference  Actual     Error");

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

			Log(this).cache(addedEnergy.toString(7) + " |   " + batches[i].second.toString(7) + "   " + act.toString(7) + "   " + std::to_string(err));
		}
		const auto mae = tErr / batches.size();

		Log(this).cache("---------+------------------------------------");
		Log(this).cache("MAE:     |   " + std::to_string(mae).substr(0, 16));
		Log<>::unnest();
		Log(this).cache("");

		if (mae > waterTemperatureThreshold)
		{
			Log(this).error("Test failed > Reactor > waterTemp: MAE={0}", mae);
			Log(this).printCache();
			passed = false;
		}
		Log(this).clearCache();
	}
	void runVolumetricTest()
	{
		if (std::abs((reactorC->getTotalVolume() - reactorC->getMaxVolume()).asStd()) > overflowLossThreshold)
		{
			Log(this).error("Test failed > Reactor > volumetrics > total_volume: expected={0}   actual={1}", reactorC->getMaxVolume().toString(), reactorC->getTotalVolume().toString());
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
			Log(this).error("Test failed > Reactor > volumetrics > overflow: Total volume loss={0}.", loss);
			passed = false;
		}

		loss = abs((reactorAfter - reactorC->getMaxVolume()).asStd());
		if(loss > overflowLossThreshold)
		{
			Log(this).error("Test failed > Reactor > volumetrics > overflow: Source volume loss={0}.", loss);
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
			Log(this).error("Test failed > SingleLayerMixture > incompatible forwarding: Incompatible reactant was not forwarded.");
			passed = false;
		}

		const auto molesBefore = reactorF->getTotalMoles();
		gasMixtureA->add(oxygen);
		if (reactorF->getTotalMoles() != molesBefore)
		{
			Log(this).error("Test failed > SingleLayerMixture > incompatible forwarding: Compatible reactant was forwarded.");
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
			Log(this).error("Test failed > ForwardContainer: Reactant was not forwarded correctly.");
			passed = false;
		}

		const auto molesBefore = reactorG->getTotalMoles();
		forwardA->add(oxygen);
		if (reactorG->getTotalMoles() != molesBefore)
		{
			Log(this).error("Test failed > ForwardContainer: Reactant was not forwarded correctly.");
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
			Log(this).error("Test failed > Reactor > determinism: given reactors were already stable.");
			testPassed = false;
			return;
		}

		for (size_t i = 0; i < 127; ++i)
		{
			double err = abs((reactorD->getPressure() - copyReactor.getPressure()).asStd());
			if (err > determinismEqualityThreshold)
			{
				Log(this).error("Test failed > Reactor > determinism: reactors have different states: pressure, loss={0}.", err);
				testPassed = false;
			}

			err = abs((reactorD->getTotalMoles() - copyReactor.getTotalMoles()).asStd());
			if (err > determinismEqualityThreshold)
			{
				Log(this).error("Test failed > Reactor > determinism: reactors have different states: total mols, loss={0}.", err);
				testPassed = false;
			}

			err = abs((reactorD->getTotalMass() - copyReactor.getTotalMass()).asStd());
			if (err > determinismEqualityThreshold)
			{
				Log(this).error("Test failed > Reactor > determinism: reactors have different states: total mass, loss={0}.", err);
				testPassed = false;
			}

			err = abs((reactorD->getTotalVolume() - copyReactor.getTotalVolume()).asStd());
			if (err > determinismEqualityThreshold)
			{
				Log(this).error("Test failed > Reactor > determinism: reactors have different states: total volume, loss={0}.", err);
				testPassed = false;
			}

			if (reactorD->hasSameContent(copyReactor, determinismEqualityThreshold) == false)
			{
				Log(this).error("Test failed > Reactor > determinism: reactors have different contents.");
				testPassed = false;
			}

			if (reactorD->hasSameLayers(copyReactor, determinismEqualityThreshold) == false)
			{
				Log(this).error("Test failed > Reactor > determinism: reactors have different layers.");
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
		Log<>::nest();
		Log(this).cache("Energy  |   SourceTemp   DestinationTemp  Source Nucleator Amount");

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
		Log(this).cache("0kJ     |   " +
			source.getTemperature().toString(8) + "    " +
			destination.getTemperature().toString(8) + "        " +
			reactorE->getAmountOf(nucleator).toString(8));

		for (size_t i = 0; i < 64; ++i)
		{
			reactorE->add(Amount<Unit::JOULE>(energyStep));
			reactorE->tick(tickTimespan);
			Log(this).cache(std::to_string(energyStep.asKilo() * (i + 1)).substr(0, 5) + "kJ |   " +
				source.getTemperature().toString(8) + "    " +
				destination.getTemperature().toString(8) + "        " +
				reactorE->getAmountOf(nucleator).toString(8));

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
					Log(this).error("Test failed > Reactor > aggregation change > max source temp exceeded, T={0}.", sTemp.toString());
					testPassed = false;
					break;
				}

				const auto dTemp = destination.getTemperature();;
				if (dTemp != pastDestinationTemp)
				{
					Log(this).error("Test failed > Reactor > aggregation change > destination temperature changed in phase 0.");
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
					Log(this).error("Test failed > Reactor > aggregation change > max destination temp exceeded, T={0}.", dTemp.toString());
					testPassed = false;
					break;
				}

				const auto sTemp = source.getTemperature();;
				if (sTemp != pastSourceTemp)
				{
					Log(this).error("Test failed > Reactor > aggregation change > source temperature changed in phase 1.");
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
					Log(this).error("Test failed > Reactor > aggregation change > nucleator did not transfer in phase 2.");
					testPassed = false;
					break;
				}
				pastNucleatorAmount = nMoles;

				const auto dTemp = destination.getTemperature();;
				if (dTemp != pastDestinationTemp)
				{
					Log(this).error("Test failed > Reactor > aggregation change > destination temperature changed in phase 2.");
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
					Log(this).error("Test failed > Reactor > aggregation change > destination temperature did not increase in phase 3.");
					testPassed = false;
					break;
				}
				pastDestinationTemp = dTemp;
				if (source.getTemperature().isInfinity() == false)
				{
					Log(this).error("Test failed > Reactor > aggregation change > empty layer temperature was not infinity.");
					testPassed = false;
					break;
				}
			}
		}

		Log(this).cache("--------+--------------------------------------------");

		if(testPassed && testPhase != 3)
		{
			Log(this).error("Test failed > Reactor > aggregation change > not all test phases were reached, phase={0}.", testPhase);
			testPassed = false;
		}

		Log<>::unnest();
		Log(this).cache("");

		if (testPassed == false)
		{
			Log(this).printCache();
			passed = false;
		}

		Log(this).clearCache();
	}

public:
	void initialize()
	{
		dumpA = new DumpContainer();
		atmosphere = new Atmosphere(
			1.0_C, 760.0_torr,
			{ { Molecule("N#N"), 78.084_mol }, { Molecule("O=O"), 20.946_mol } },
			1000.0_L, *dumpA);

		reactorA = new Reactor(*atmosphere, 1.0_L);
		reactorA->add(Molecule("HH"), 2.0_mol);
		reactorA->add(Molecule("CC=C"), 2.0_mol);
		reactorA->add(Molecule("CC(=O)OCC"), 2.0_mol);
		reactorA->add(Molecule("O"), 3.0_mol);

		reactorB = new Reactor(*atmosphere, 1.0_L);
		reactorB->setTickMode(reactorB->getTickMode() - TickMode::ENABLE_CONDUCTION);
		reactorB->add(Molecule("O"), 3.0_mol);

		reactorC = new Reactor(*atmosphere, 20.0_L);

		reactorD = new Reactor(*atmosphere, 5.0_L);
		reactorD->setTickMode(reactorD->getTickMode());
		reactorD->add(Molecule("CC(=O)O"), 2.0_mol);
		reactorD->add(Molecule("OCC"), 3.0_mol);

		reactorE = new Reactor(*atmosphere, 0.1_L);
		reactorE->setTickMode(reactorE->getTickMode() - TickMode::ENABLE_CONDUCTION);
		reactorE->add(Molecule("O"), 5.4_mol);
		
		reactorF = new Reactor(*atmosphere, 1.0_L);
		gasMixtureA = new SingleLayerMixture<LayerType::GASEOUS>(
			1.0_C, 760.0_torr, { { Molecule("N#N"), 78.084_mol } }, 0.5_L, *dumpA);
		gasMixtureA->setIncompatibilityTarget(LayerType::POLAR, *reactorF);

		reactorG = new Reactor(*atmosphere, 1.0_L);
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
		Log<>::nest();
		Accessor<>::setDataStore(store);

		const auto begin = std::chrono::steady_clock::now();
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
		Log(this).info("Test initialization completed in {0}s.",
			std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0);
	}

	~TestManager()
	{
		Log<>::unnest();
	}

	void runAll()
	{
		const auto begin = std::chrono::steady_clock::now();
		molecularStructureTest.runTests();
		estimatorTest.runTests();
		reactorTest.runTests();
		const auto end = std::chrono::steady_clock::now();

		Log(this).info("Test execution completed in {0}s.", 
			std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0);

		if (molecularStructureTest.hasPassed())
			Log(this).success("All MolecularStructure tests passed.");
		if (estimatorTest.hasPassed())
			Log(this).success("All Estimator tests passed.");
		if (reactorTest.hasPassed())
			Log(this).success("All Reactor tests passed.");
	}

	void runPersist()
	{
		const auto begin = std::chrono::steady_clock::now();
		//store.reactions.generateTotalSpan();
		store.saveGenericMoleculesData("Out/genericmolecules.out.csv")
			.saveMoleculesData("Out/molecules.out.csv");
		const auto end = std::chrono::steady_clock::now();

		Log(this).info("Total span dump completed in {0}s.",
			std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0);
	}
};
