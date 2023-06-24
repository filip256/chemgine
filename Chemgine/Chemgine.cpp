#include <iostream>
#include <vector>
#include <string>

#include "DataStore.hpp"
#include "Logger.hpp"
#include "Atom.hpp"
#include "CompositeComponent.hpp"








int main()
{
    DataStore r;
    r.loadAtomsData("Data/AtomData.csv");
    BaseComponent::setDataStore(&r);

    MolecularStructure c("CCCCCC");
    std::cout<<c.print();
    
    getchar();
    return 0;
}
