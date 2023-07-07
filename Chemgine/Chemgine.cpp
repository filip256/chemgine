#include <iostream>
#include <vector>
#include <string>

#include "DataStore.hpp"
#include "Logger.hpp"
#include "Atom.hpp"
#include "CompositeComponent.hpp"


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
        DataStore r;
        BaseComponent::setDataStore(r);
        r.loadAtomsData("Data/AtomData.csv");
        r.loadFunctionalGroupsData("Data/FunctionalGroupData.csv");
        r.loadBackbonesData("Data/BackboneData.csv");
        r.loadMoleculesData("Data/OrganicMoleculeData.csv");

        MolecularStructure a("RCO"), b("CCO");
        std::vector<uint8_t> vect(3, false);
        std::cout << MolecularStructure::marchCompare(0, a, vect, 1, b);
    }

    if (BaseComponent::instanceCount != 0)
        Logger::log("Memory leak detected: BaseComponent (" + std::to_string(BaseComponent::instanceCount) + " unreleased instances).", LogType::BAD);

    getchar();
    return 0;
}
