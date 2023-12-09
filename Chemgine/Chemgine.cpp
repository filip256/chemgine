#include <iostream>
#include <vector>
#include <string>

#include "DataStore.hpp"
#include "Logger.hpp"
#include "Atom.hpp"
#include "CompositeComponent.hpp"
#include "Tests.hpp"
#include "Reactable.hpp"

#include "PVector.hpp"
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

/*
FunctionalGroup {
    CompositeComponent {
        BaseComponent {
            ComponentType,
            DataStoreAccessor {
                DataStore {
                    AtomDataTable {
                        DataTable { MultiIndexMap },
                        AtomData { BaseComponentData }
                    }
                    FunctionalGroupDataTable {
                        DataTable { MultiIndexMap },
                        FunctionalGroupData {
                            BaseComponentData,
                            MolecularStructure {
                                Bond{
                                    BondType,
                                    Atom {
                                        AtomicComponent { BaseComponent... }
                                        AtomData { BaseComponentData }
                                    }
                                },
                                BaseComponent...
                            }
                        }
                    }
                }
            }
        }
    }
}
*/

int main()
{
    {
#ifndef NDEBUG
        TestManager tests;
        tests.runAll();
#endif


        DataStore store;
        BaseComponent::setDataStore(store);
        Reactable::setDataStore(store);
        Reactor::setDataStore(store);
        Molecule::setDataStore(store);
        BaseLabwareComponent::setDataStore(store);
        store.loadAtomsData("Data/AtomData.csv")
            .loadFunctionalGroupsData("Data/FunctionalGroupData.csv")
            .loadBackbonesData("Data/BackboneData.csv")
            .loadApproximatorsData("")
            .loadMoleculesData("Data/MoleculeData.csv")
            .loadReactionsData("Data/ReactionData.csv")
            .loadLabwareData("Data/LabwareData.csv");

        //UIContext uiContext;
        //uiContext.run();

        Reactor reactor(20.0, 760.0);
        //reactor.add(Molecule("HH"), 1.0);
        //reactor.add(Molecule("CC=C"), 1.0);
        //reactor.add(Molecule("CCC(=O)O"), 1.0);
        reactor.add(Molecule("CC(=O)O"), 1.0);
        reactor.add(Molecule("CO"), 2.0);
        //reactor.add(Molecule("CCCO"), 2.0);
        //reactor.add(Molecule("C=CCO"), 2.0);

        while (true)
        {
            reactor.tick();
        }

        //MolecularStructure a("O=C(OC)C");
        //MolecularStructure a("CC(=O)OC");
        //MolecularStructure b("OCC");
        //std::cout << a.print() << '\n' << b.print() << '\n';
        //std::cout << MolecularStructure("CC1COC2CN(C)C(C)C3OCC1C23").serialize()<<'\n';
        //std::cout << MolecularStructure("C1C2CC12").toSMILES() << '\n';
    }

    if (BaseComponent::instanceCount != 0)
        Logger::log("Memory leak detected: BaseComponent (" + std::to_string(BaseComponent::instanceCount) + " unreleased instances).", LogType::BAD);
    if (Bond::instanceCount != 0)
        Logger::log("Memory leak detected: Bond (" + std::to_string(Bond::instanceCount) + " unreleased instances).", LogType::BAD);
    if (BaseLabwareData::instanceCount != 0)
        Logger::log("Memory leak detected: BaseLabwareData (" + std::to_string(BaseLabwareData::instanceCount) + " unreleased instances).", LogType::BAD);
    if (BaseLabwareComponent::instanceCount != 0)
        Logger::log("Memory leak detected: BaseLabwareComponent (" + std::to_string(BaseLabwareComponent::instanceCount) + " unreleased instances).", LogType::BAD);
    if (BaseApproximator::instanceCount != 0)
        Logger::log("Memory leak detected: BaseApproximator (" + std::to_string(BaseApproximator::instanceCount) + " unreleased instances).", LogType::BAD);

    getchar();
    return 0;
}
