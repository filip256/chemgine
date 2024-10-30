#include "Log.hpp"

LogType LogBase::logLevel = LogType::ALL;
LogType LogBase::printNameLevel = LogType::ALL;
LogType LogBase::printAddressLevel = LogType::WARN;

std::regex LogBase::logSourceFilter(".*");

std::ostream& LogBase::outputStream = std::cout;
