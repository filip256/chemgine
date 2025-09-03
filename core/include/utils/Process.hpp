#pragma once

#include <bitset>
#include <cstdint>

namespace OS
{

enum class ProcessPriority : uint32_t
{
    // https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setpriorityclass?redirectedfrom=MSDN#parameters

    REALTIME_PRIORITY_CLASS     = 0x00000100,
    HIGH_PRIORITY_CLASS         = 0x00000080,
    ABOVE_NORMAL_PRIORITY_CLASS = 0x00008000,
    NORMAL_PRIORITY_CLASS       = 0x00000020,
    BELOW_NORMAL_PRIORITY_CLASS = 0x00004000,
    IDLE_PRIORITY_CLASS         = 0x00000040,
};

ProcessPriority getCurrentProcessPriority();
void            setCurrentProcessPriority(const ProcessPriority priority);

constexpr uint8_t MaxPocressorCount = 64;
using ProcessorIndex                = uint8_t;
using ProcessorAffinityMask         = std::bitset<MaxPocressorCount>;

ProcessorAffinityMask getAvailableProcessorMask();
ProcessorAffinityMask getAvailablePhysicalProcessorMask();
ProcessorAffinityMask setCurrentThreadProcessorAffinity(const ProcessorAffinityMask mask);

class ExecutionConfig
{
public:
    ProcessorAffinityMask processorAffinityMask;
    ProcessPriority       processPriority;

    ExecutionConfig(ProcessorAffinityMask processorAffinityMask, const ProcessPriority processPriority) noexcept;
    ExecutionConfig(const ExecutionConfig&) = default;
};

}  // namespace OS
