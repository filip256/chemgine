#include <iostream>
#include <vector>
#include <string>

#include "DataStore.hpp"
#include "Logger.hpp"
#include "Atom.hpp"








int main()
{
    DataStore r;
    r.loadAtomsData("Data/AtomData.csv");
    BaseComponent::setDataStore(&r);

    Atom a(206);

    return 0;
}
