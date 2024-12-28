#include "DataStore.hpp"
#include "UIContext.hpp"

#include "Units.hpp"

int main()
{
    LogBase::logLevel = LogType::DEBUG;

    DataStore store;
    Accessor<>::setDataStore(store);

    store.load("./../Data/builtin.cdef");

    std::cout << '\n' << store.reactions.getNetwork().print() << '\n';

    std::cout << MolecularStructure("S(=O)(=O)(O)O").print() << '\n';
    std::cout << MolecularStructure("S(-O)(-O)(-O)(OCC)(OCCC(N(C)C)=O)C#N").print() << '\n';

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
