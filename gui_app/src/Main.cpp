#include "data/DataStore.hpp"
#include "UIContext.hpp"

#include "utils/Bin.hpp"

int main()
{
    LogBase::settings().logLevel = LogType::INFO;

    DataStore store;
    Accessor<>::setDataStore(store);
    store.load("./data/builtin.cdef");

    //MolecularStructure("C12CCC1C2").getMinimalCycles();
    const auto d = MolecularStructure("C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67").getMinimalCycleBasis();
    Log().info(MolecularStructure("COC1C=C2C(=CC=1)NCC2CCN(C)C").printInfo());

    //MolecularStructure c1("NC2C1NCC=12");
    //MolecularStructure c2("NC2C1NCC12");
    MolecularStructure c3("C3=CC27CC18C=CC16C=C%10CCC%12C%11C=C5C=C4C(C=C2C3)C49C5=C(C6C789)C%10%11%12");
    MolecularStructure c4("N1(C2(C1C2(C)))");
    std::cout << c3.toSMILES() << '\n';
    std::cout << c4.toSMILES() << '\n';
    std::cout << MolecularStructure("CC1C2C1N2").toSMILES() << '\n';
    std::cout << MolecularStructure("C1=CC=CC=C1").toSMILES() << '\n';
    std::cout << MolecularStructure("CC1C2CC12").toSMILES() << '\n';
    std::cout << MolecularStructure("CC1CC1C").toSMILES() << '\n';
    std::cout << MolecularStructure("CC(CC)C").toSMILES() << '\n';
    std::cout << MolecularStructure("C(CC)C").toSMILES() << '\n';
    std::cout << MolecularStructure("C(CC)(CCC)C").toSMILES() << '\n';
    std::cout << MolecularStructure("C(C(CC)C)C").toSMILES() << '\n';
    std::cout << MolecularStructure("C(CC)(C(CC)CC)C").toSMILES() << '\n';

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
