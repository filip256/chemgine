#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "DataStore.hpp"
#include "Log.hpp"
#include "Atom.hpp"
#include "Tests.hpp"
#include "StructureRef.hpp"

#include "Reactor.hpp"
#include "Query.hpp"

#include "Amount.hpp"
#include "BaseLabwareData.hpp"
#include "BaseLabwareComponent.hpp"
#include "LabwareSystem.hpp"
#include "StructureRef.hpp"

#include "UIContext.hpp"

#include "ContainsInterface.hpp"

#include "SystemMatrix.hpp"

#include "Spline.hpp"
#include "FlagField.hpp"
#include "UndirectedGraph.hpp"
#include "TextBlock.hpp"
#include "Linguistics.hpp"
#include "DynamicAmount.hpp"
#include "PathUtils.hpp"

#include <algorithm> 

int main()
{
    {
        LogBase::logLevel = LogType::DEBUG;

        {
            TestManager tests;
            tests.runAll();
            tests.runPersist();
        }

        DataStore store;
        Accessor<>::setDataStore(store);

        store.load("./Data/builtin.cdef");
        store.dump("./Out/builtin.cdef");
        store.clear();
        store.load("./Out/builtin.cdef");

        std::cout << MolecularStructure("S(=O)(=O)(O)O").print() << '\n';

        std::cout << MolecularStructure("S(-O)(-O)(-O)(OCC)(OCCC(N(C)C)=O)C#N").print()<<'\n';

        const auto x = store.reactions.getRetrosynthReactions(
            *StructureRef::create("O(C(C)C)C(=O)C"));

        for (const auto& i : x)
        {
            i.print();
            std::cout << "-------------------------\n\n";
        }

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

    if (BaseLabwareComponent::instanceCount != 0)
        Log().error("Memory leak detected: BaseLabwareComponent ({0} unreleased instances).", BaseLabwareComponent::instanceCount);
    if (BaseContainer::instanceCount != 0)
        Log().error("Memory leak detected: BaseContainer ({0} unreleased instances).", BaseContainer::instanceCount);

    getchar();
    return 0;
}
