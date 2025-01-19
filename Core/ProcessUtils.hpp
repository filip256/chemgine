#pragma once

#include <cstdint>

namespace OS
{
	enum class ProcessPriority : uint32_t
	{
		// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setpriorityclass?redirectedfrom=MSDN#parameters
		
		ABOVE_NORMAL_PRIORITY_CLASS = 0x00008000,
		BELOW_NORMAL_PRIORITY_CLASS = 0x00004000,
		
		HIGH_PRIORITY_CLASS = 0x00000080,
		IDLE_PRIORITY_CLASS = 0x00000040,
		NORMAL_PRIORITY_CLASS = 0x00000020,
		REALTIME_PRIORITY_CLASS = 0x00000100,
		
		PROCESS_MODE_BACKGROUND_BEGIN = 0x00100000,
		PROCESS_MODE_BACKGROUND_END = 0x00200000,
	};

	ProcessPriority getCurrentProcessPriority();
	void setCurrentProcessPriority(ProcessPriority priority);
}
