#include <iostream>
#include <vector>
#include <string>

#include "DataStore.hpp"
#include "Logger.hpp"
#include "Atom.hpp"
#include "CompositeComponent.hpp"
#include "Tests.hpp"
#include "Reactable.hpp"
#include "ReactableFactory.hpp"

#include "PVector.hpp"
#include "Reactor.hpp"
#include "Query.hpp"


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
        TestManager tests;
        tests.runAll();


        DataStore r;
        BaseComponent::setDataStore(r);
        ReactableFactory::setDataStore(r);
        Reactor::setDataStore(r);
        r.loadAtomsData("Data/AtomData.csv");
        r.loadFunctionalGroupsData("Data/FunctionalGroupData.csv");
        r.loadBackbonesData("Data/BackboneData.csv");
        r.loadMoleculesData("Data/OrganicMoleculeData.csv");
        r.loadReactionsData("Data/ReactionData.csv");



        //MolecularStructure a("O=C(OC)C");
        MolecularStructure a("CC(=O)OC");
        MolecularStructure b("OCC");
        std::cout << a.print() << '\n' << b.print() << '\n';
        //std::cout << MolecularStructure("CC1COC2CN(C)C(C)C3OCC1C23").toSMILES()<<'\n';
        //std::cout << MolecularStructure("C1C2CC12").toSMILES() << '\n';

        std::cout << MolecularStructure(MolecularStructure("CC1COC2CN(C)C(C)C3OCC1C23").serialize(), false).print()<<'\n';

    }

    if (BaseComponent::instanceCount != 0)
        Logger::log("Memory leak detected: BaseComponent (" + std::to_string(BaseComponent::instanceCount) + " unreleased instances).", LogType::BAD);
    if (Bond::instanceCount != 0)
        Logger::log("Memory leak detected: Bond (" + std::to_string(Bond::instanceCount) + " unreleased instances).", LogType::BAD);
    if (Reactable::instanceCount != 0)
        Logger::log("Memory leak detected: Reactable (" + std::to_string(Reactable::instanceCount) + " unreleased instances).", LogType::BAD);

    getchar();
    return 0;
}
