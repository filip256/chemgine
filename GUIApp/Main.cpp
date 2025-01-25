#include "DataStore.hpp"
#include "UIContext.hpp"

int main()
{
    LogBase::logLevel = LogType::DEBUG;

    DataStore store;
    Accessor<>::setDataStore(store);

    store.load("./../Data/builtin.cdef");

    std::cout << MolecularStructure("NC1C2=C1CN2").toSMILES() << '\n';

    std::cout << '\n' << store.reactions.getNetwork().print() << '\n';

    const auto x = store.reactions.getRetrosynthReactions(
        *StructureRef::create("O(C(C)C)C(=O)C"));
    for (const auto& i : x)
    {
        i.print();
        std::cout << "-------------------------\n\n";
    }

    UIContext uiContext;
    uiContext.run();

    return 0;
}
