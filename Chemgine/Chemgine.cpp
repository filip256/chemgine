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
*/






int main()
{
    {
        DataStore r;
        BaseComponent::setDataStore(r);
        r.loadAtomsData("Data/AtomData.csv");
        r.loadFunctionalGroupsData("Data/FunctionalGroupData.csv");
        r.loadBackbonesData("Data/BackboneData.csv");

        MolecularStructure c("CO[Na]");
        std::cout << c.print() << '\n';
        std::cout << c.getMolarMass();
    }

    if (BaseComponent::instanceCount != 0)
        Logger::log("Memory leak detected: BaseComponent (" + std::to_string(BaseComponent::instanceCount) + " unreleased instances).", LogType::BAD);

    getchar();
    return 0;
}
