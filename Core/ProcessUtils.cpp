#include "ProcessUtils.hpp"
#include "Casts.hpp"
#include "Log.hpp"

#include <Windows.h>

OS::ProcessPriority OS::getCurrentProcessPriority()
{
	const auto priority = GetPriorityClass(GetCurrentProcess());
	if(!priority)
		Log().fatal("Failed to get process priority.");

	return static_cast<ProcessPriority>(priority);
}

void OS::setCurrentProcessPriority(OS::ProcessPriority priority)
{
	if (!SetPriorityClass(GetCurrentProcess(), static_cast<DWORD>(priority)))
		Log().fatal("Failed to set process priority: {0}.", underlying_cast(priority));
}
