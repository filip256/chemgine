#include <iostream>
#include <vector>
#include <string>

#include "DataStore.hpp"
#include "Logger.hpp"
#include "Atom.hpp"
#include "CompositeComponent.hpp"








int main()
{
    {
        DataStore r;
        r.loadAtomsData("Data/AtomData.csv");
        r.loadAtomsData("Data/FunctionalGroupData.csv");
        BaseComponent::setDataStore(&r);

        MolecularStructure c("CO[Na]");
        std::cout << c.print() << '\n';
        std::cout << c.getMolarMass();
    }

    if (BaseComponent::instanceCount != 0)
        Logger::log("Memory leak detected: BaseComponent (" + std::to_string(BaseComponent::instanceCount) + " unreleased instances).", LogType::BAD);

    getchar();
    return 0;
}
