#include <iostream>
#include <vector>
#include <string>

#include "DataStore.hpp"
#include "Logger.hpp"
#include "Atom.hpp"
#include "CompositeComponent.hpp"
#include "Tests.hpp"


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
        //TestManager tests;
        //tests.runAll();

        DataStore r;
        BaseComponent::setDataStore(r);
        r.loadAtomsData("Data/AtomData.csv");
        MolecularStructure a("CC(=O)OC");
        MolecularStructure b("CO");
        std::cout << a.print() << '\n' << b.print() << '\n';
        auto map = a.maximalMapTo(b);
        for (auto const& x : map)
        {
            std::cout << x.first  // string (key)
                << ':'
                << x.second // string's value 
                << std::endl;
        }
    }

    if (BaseComponent::instanceCount != 0)
        Logger::log("Memory leak detected: BaseComponent (" + std::to_string(BaseComponent::instanceCount) + " unreleased instances).", LogType::BAD);
    if (Bond::instanceCount != 0)
        Logger::log("Memory leak detected: Bond (" + std::to_string(Bond::instanceCount) + " unreleased instances).", LogType::BAD);

    getchar();
    return 0;
}
