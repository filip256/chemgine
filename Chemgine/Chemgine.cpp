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

        MolecularStructure a("CC(=O)OC"), b("RC(=O)OR");  // true
        //MolecularStructure a("CC(=O)OC"), b("RC(=O)O");  // false
        //MolecularStructure a("CC(=O)N(C)C"), b("RC(=O)N(R)R");  // true
        //MolecularStructure a("O(C)(CC)"), b("O(C)(C)");  // false
        //MolecularStructure a("C1CC1"), b("C1CC1"); // true
        //MolecularStructure a("C1CCC1"), b("C1CC1"); // false
        //MolecularStructure a("C1C(OC)CC1"), b("C1CC(OR)C1"); // true
        //MolecularStructure a("CC(O)C"), b("OR"); // true
        std::cout << a.print()<<'\n';
        std::cout << b.print();

        std::cout << MolecularStructure::isPartOf(a, b)<<'\n';
    }

    if (BaseComponent::instanceCount != 0)
        Logger::log("Memory leak detected: BaseComponent (" + std::to_string(BaseComponent::instanceCount) + " unreleased instances).", LogType::BAD);

    getchar();
    return 0;
}
