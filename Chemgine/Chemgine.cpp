#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "DataStore.hpp"
#include "Logger.hpp"
#include "Atom.hpp"
#include "CompositeComponent.hpp"
#include "Tests.hpp"
#include "Reactable.hpp"

#include "Reactor.hpp"
#include "Query.hpp"

#include "Amount.hpp"
#include "BaseLabwareData.hpp"
#include "BaseLabwareComponent.hpp"
#include "LabwareSystem.hpp"
#include "Reactable.hpp"

#include "UIContext.hpp"

#include "ContainsInterface.hpp"

#include "SystemMatrix.hpp"

#include "Spline.hpp"
#include "FlagField.hpp"
#include "UndirectedGraph.hpp"
#include "TextBlock.hpp"
#include "Linguistics.hpp"
#include "DynamicAmount.hpp"

int main()
{
    {
        //#ifndef NDEBUG
        {
            TestManager tests;
            tests.runAll();
            tests.runPersist();
        }
        //#endif

        DataStore store;
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

        std::cout << MolecularStructure("S(-O)(-O)(-O)(OCC)(OCCC(N(C)C)=O)C#N").print();

        const auto x = store.reactions.getRetrosynthReactions(
            *Reactable::get(MolecularStructure("CC(=O)OCCC")));

        //for (const auto& i : x)
        //{
        //    std::cout << i.getBaseData().getHRTag() << '\n';
        //    for (const auto& r : i.getReactants())
        //        std::cout << r.first.getStructure().print() << '\n';
        //    std::cout << '\n';
        //    for (const auto& p : i.getProducts())
        //        std::cout << p.first.getStructure().print() << '\n';
        //    std::cout << "-------------------------\n";
        //}

        UIContext uiContext;
        uiContext.run();

        //std::cout << MolecularStructure("OC1C2CC12").toSMILES() << '\n';

        //const auto& ra = store.reactions.at(201);
        //const auto& rb = store.reactions.at(207);

        //std::cout<<rb.isSpecializationOf(ra)<<'\n';

        //Reactor reactor(20.0, 760.0);
        //reactor.add(Molecule("HH"), 1.0);
        //reactor.add(Molecule("CC=C"), 1.0);
        //reactor.add(Molecule("CCC(=O)O"), 3.0);
        //reactor.add(Molecule("CC(=O)O"), 2.0);
        //reactor.add(Molecule("CO"), 2.0);
        //reactor.add(Molecule("CCCO"), 2.0);
        //reactor.add(Molecule("C=CCO"), 2.0);

        //while (true)
        //{
        //    const auto begin = std::chrono::steady_clock::now();
        //    reactor.tick();
        //    std::cout << "Tick in: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count() / 1000000.0) + "s.\n";
        //}

        //MolecularStructure a("O=C(OC)C");
        //MolecularStructure a("CC(=O)OC");
        //MolecularStructure b("OCC");
        //std::cout << a.print() << '\n' << b.print() << '\n';
        //std::cout << MolecularStructure("CC1COC2CN(C)C(C)C3OCC1C23").serialize()<<'\n';
        //std::cout << MolecularStructure("C1C2CC12").toSMILES() << '\n';

        //store.saveGenericMoleculesData("Out/genericmolecules.out.csv")
        //    .saveMoleculesData("Out/molecules.out.csv");
    }

    if (BaseComponent::instanceCount != 0)
        Logger::log("Memory leak detected: BaseComponent (" + std::to_string(BaseComponent::instanceCount) + " unreleased instances).", LogType::BAD);
    if (Bond::instanceCount != 0)
        Logger::log("Memory leak detected: Bond (" + std::to_string(Bond::instanceCount) + " unreleased instances).", LogType::BAD);
    if (BaseLabwareData::instanceCount != 0)
        Logger::log("Memory leak detected: BaseLabwareData (" + std::to_string(BaseLabwareData::instanceCount) + " unreleased instances).", LogType::BAD);
    if (BaseLabwareComponent::instanceCount != 0)
        Logger::log("Memory leak detected: BaseLabwareComponent (" + std::to_string(BaseLabwareComponent::instanceCount) + " unreleased instances).", LogType::BAD);
    if (BaseEstimator::instanceCount != 0)
        Logger::log("Memory leak detected: BaseEstimator (" + std::to_string(BaseEstimator::instanceCount) + " unreleased instances).", LogType::BAD);
    if (BaseContainer::instanceCount != 0)
        Logger::log("Memory leak detected: BaseContainer (" + std::to_string(BaseContainer::instanceCount) + " unreleased instances).", LogType::BAD);

    getchar();
    return 0;
}
