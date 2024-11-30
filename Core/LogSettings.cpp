#include "Log.hpp"

LogType LogBase::logLevel = LogType::ALL;
LogType LogBase::printNameLevel = LogType::ALL;
LogType LogBase::printAddressLevel = LogType::WARN;

std::string LogBase::contexIndent = ".   ";

std::regex LogBase::logSourceFilter(".*", std::regex::optimize);

std::ostream& LogBase::outputStream = std::cout;
