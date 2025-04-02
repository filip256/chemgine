#include "Log.hpp"

LogType LogBase::logLevel = LogType::ALL;
FlagField<LogType> LogBase::printNameLevel = FlagField(LogType::DEBUG) | LogType::WARN | LogType::ERROR | LogType::FATAL;
FlagField<LogType> LogBase::printAddressLevel = FlagField(LogType::DEBUG) | LogType::WARN | LogType::ERROR | LogType::FATAL;
FlagField<LogType> LogBase::printLocationLevel = FlagField(LogType::DEBUG) | LogType::WARN | LogType::ERROR | LogType::FATAL;

std::string LogBase::contextIndent = "ù   ";

std::regex LogBase::logSourceFilter(".*", std::regex::optimize);

std::ostream& LogBase::outputStream = std::cout;
