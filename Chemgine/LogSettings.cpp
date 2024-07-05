#include "Log.hpp"

LogType LogBase::logLevel = LogType::ALL;
LogType LogBase::printNameLevel = LogType::WARN;
LogType LogBase::printAddressLevel = LogType::ERROR;

std::ostream& LogBase::outputStream = std::cout;
