#include "Atom.hpp"
#include "Logger.hpp"


Atom::Atom(const AtomIdType id) :
    BaseComponent(),
    id(id)
{
    if (dataStore->atoms.contains(id) == false)
        Logger::log("Atom with id " + std::to_string(id) + " could not be found.", LogType::BAD);
}