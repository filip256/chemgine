#include <iostream>
#include <vector>
#include <string>

#include "DataStore.hpp"
#include "Logger.hpp"








int main()
{
    //Logger::log("Hello World!");
    //Logger::log("Hello World!", LogType::FATAL);
    //Logger::log("Hello World!", LogType::BAD);
    //Logger::log("Hello World!", LogType::WARN);
    //Logger::log("Hello World!", LogType::GOOD);
    //Logger::breakline();

    DataStore r;
    r.loadAtomsData("Data/AtomData.csv");
}
