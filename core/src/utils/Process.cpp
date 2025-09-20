#include "utils/Process.hpp"

#include "io/Log.hpp"
#include "utils/Build.hpp"
#include "utils/Casts.hpp"

#include <array>
#include <charconv>
#include <fstream>

#if defined(CHG_BUILD_WINDOWS)
    #include <Windows.h>
#elif defined(CHG_BUILD_LINUX)
    #include <sched.h>
    #include <sys/resource.h>
    #include <unistd.h>

    #include <cerrno>
#endif

#ifdef CHG_BUILD_WINDOWS

namespace Windows
{

class ProcessorInfo
{
private:
    OS::ProcessorIndex                                                      count;
    std::array<SYSTEM_LOGICAL_PROCESSOR_INFORMATION, OS::MaxPocressorCount> info;

    ProcessorInfo();
    ProcessorInfo(const ProcessorInfo&) = delete;
    ProcessorInfo(ProcessorInfo&&)      = delete;

    static const ProcessorInfo processorInfo;

public:
    static OS::ProcessorIndex                   getCount();
    static SYSTEM_LOGICAL_PROCESSOR_INFORMATION getInfo(const OS::ProcessorIndex idx);
};

const ProcessorInfo ProcessorInfo::processorInfo;

ProcessorInfo::ProcessorInfo()
{
    auto length = static_cast<DWORD>(info.size() * sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
    if (not GetLogicalProcessorInformation(info.data(), &length))
        Log().fatal("Failed to get processor information (error code: {}).", GetLastError());

    count = static_cast<OS::ProcessorIndex>(length / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
}

OS::ProcessorIndex ProcessorInfo::getCount() { return processorInfo.count; }

SYSTEM_LOGICAL_PROCESSOR_INFORMATION ProcessorInfo::getInfo(const OS::ProcessorIndex idx)
{
    if (idx >= processorInfo.count)
        Log().fatal("Invalid processor index: {}, system only has: {} processors.", idx, processorInfo.count);

    return processorInfo.info[idx];
}

}  // namespace Windows

#elif defined(CHG_BUILD_LINUX)

namespace Linux
{

using ProcessPriority = int8_t;  // -20 -> 19

ProcessPriority priorityCast(const OS::ProcessPriority priority)
{
    switch (priority) {
    case OS::ProcessPriority::REALTIME_PRIORITY_CLASS:
        return -20;
    case OS::ProcessPriority::HIGH_PRIORITY_CLASS:
        return -15;
    case OS::ProcessPriority::ABOVE_NORMAL_PRIORITY_CLASS:
        return -10;
    case OS::ProcessPriority::NORMAL_PRIORITY_CLASS:
        return 0;
    case OS::ProcessPriority::BELOW_NORMAL_PRIORITY_CLASS:
        return 10;
    case OS::ProcessPriority::IDLE_PRIORITY_CLASS:
        return 19;
    default:
        Log().fatal("Unsupported process priority: {}.", underlying_cast(priority));
    }
}

OS::ProcessPriority priorityCast(const ProcessPriority priority)
{
    if (priority < -15)
        return OS::ProcessPriority::REALTIME_PRIORITY_CLASS;
    if (priority < -10)
        return OS::ProcessPriority::HIGH_PRIORITY_CLASS;
    if (priority < 0)
        return OS::ProcessPriority::ABOVE_NORMAL_PRIORITY_CLASS;
    if (priority == 0)
        return OS::ProcessPriority::NORMAL_PRIORITY_CLASS;
    if (priority <= 10)
        return OS::ProcessPriority::BELOW_NORMAL_PRIORITY_CLASS;
    return OS::ProcessPriority::IDLE_PRIORITY_CLASS;
}

using ProcessorAffinityMask = cpu_set_t;

ProcessorAffinityMask affinityMaskCast(const OS::ProcessorAffinityMask mask)
{
    ProcessorAffinityMask result;
    CPU_ZERO(&result);
    for (OS::ProcessorIndex cpu = 0; cpu < OS::MaxPocressorCount; ++cpu) {
        if (mask.test(cpu)) {
            CPU_SET(cpu, &result);
        }
    }
    return result;
}

OS::ProcessorAffinityMask affinityMaskCast(const ProcessorAffinityMask mask)
{
    OS::ProcessorAffinityMask result;
    for (OS::ProcessorIndex cpu = 0; cpu < OS::MaxPocressorCount; ++cpu) {
        if (CPU_ISSET(cpu, &mask)) {
            result.set(cpu);
        }
    }
    return result;
}

}  // namespace Linux

#endif

OS::ProcessPriority OS::getCurrentProcessPriority()
{
#ifdef CHG_BUILD_WINDOWS
    const auto priority = GetPriorityClass(GetCurrentProcess());
    if (not priority)
        Log().fatal("Failed to get process priority (error code: {}).", GetLastError());

    return static_cast<ProcessPriority>(priority);

#else
    errno              = 0;
    const auto niceVal = getpriority(PRIO_PROCESS, 0);
    if (niceVal == -1 && errno != 0)
        Log().fatal("Failed to get process priority (error code: {}).", errno);

    return Linux::priorityCast(niceVal);
#endif
}

void OS::setCurrentProcessPriority(const OS::ProcessPriority priority)
{
#ifdef CHG_BUILD_WINDOWS
    if (not SetPriorityClass(GetCurrentProcess(), static_cast<DWORD>(priority)))
        Log().fatal("Failed to set process priority: {} (error code: {}).", underlying_cast(priority), GetLastError());

#else
    const auto niceVal = Linux::priorityCast(priority);
    if (setpriority(PRIO_PROCESS, 0, niceVal) != 0)
        Log().fatal("Failed to set process priority: {} (error code: {}).", underlying_cast(priority), errno);

    if (priority == ProcessPriority::REALTIME_PRIORITY_CLASS) {
        static const sched_param param{sched_get_priority_max(SCHED_FIFO)};
        if (sched_setscheduler(0, SCHED_FIFO, &param) != 0)
            Log().fatal("Failed to set SCHED_FIFO scheduler (error code: {}).", errno);
    }
    else {
        static const sched_param param{};
        if (sched_setscheduler(0, SCHED_OTHER, &param) != 0)
            Log().fatal("Failed to set SCHED_OTHER scheduler (error code: {}).", errno);
    }
#endif

    Log().debug("Set process priority: {}.", underlying_cast(priority));
}

OS::ProcessorAffinityMask OS::getAvailableProcessorMask()
{
#ifdef CHG_BUILD_WINDOWS
    DWORD_PTR processAffinityMask, systemAffinityMask;
    if (not GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask, &systemAffinityMask))
        Log().fatal("Failed to get process affinity mask (error code: {}).", GetLastError());

    return static_cast<ProcessorAffinityMask>(processAffinityMask);

#else
    Linux::ProcessorAffinityMask mask;
    CPU_ZERO(&mask);
    if (sched_getaffinity(0, sizeof(mask), &mask) == -1)
        Log().fatal("Failed to get process affinity mask (error code: {}).", errno);

    return Linux::affinityMaskCast(mask);
#endif
}

OS::ProcessorAffinityMask OS::getAvailablePhysicalProcessorMask()
{
    static const auto     availableProcessors = getAvailableProcessorMask();
    ProcessorAffinityMask physicalProcessors  = {};

#ifdef CHG_BUILD_WINDOWS

    for (OS::ProcessorIndex cpu = 0; cpu < OS::MaxPocressorCount; ++cpu) {
        if (availableProcessors.test(cpu) && Windows::ProcessorInfo::getInfo(cpu).Relationship == RelationProcessorCore)
            physicalProcessors.set(cpu);
    }

    return physicalProcessors;

#else
    for (OS::ProcessorIndex cpu = 0; cpu < OS::MaxPocressorCount; ++cpu) {
        if (not availableProcessors.test(cpu))
            continue;

        // Read thread siblings which has the following form:
        // '<CPU_ID><SEP><CPU_ID><SEP>...'
        // If the first sibling is the same as the current CPU then the CPU is physical.
        std::ifstream file("/sys/devices/system/cpu/cpu" + std::to_string(cpu) + "/topology/thread_siblings_list");
        if (!file)
            continue;

        std::string line;
        std::getline(file, line);
        file.close();

        ProcessorIndex parsedCpu = 0;
        const auto [_, error]    = std::from_chars(line.data(), line.data() + line.size(), parsedCpu);
        if (error != std::errc())
            continue;
        if (cpu != parsedCpu)
            continue;  // Not a physical core.

        physicalProcessors.set(cpu);
    }

    return availableProcessors;
#endif
}

OS::ProcessorAffinityMask OS::setCurrentThreadProcessorAffinity(const ProcessorAffinityMask mask)
{
#ifdef CHG_BUILD_WINDOWS
    const auto prevMask = SetThreadAffinityMask(GetCurrentThread(), static_cast<DWORD_PTR>(mask.to_ullong()));
    if (not prevMask)
        Log().fatal(
            "Failed to set thread affinity to mask: '{}' (error code: {}).", mask.to_string('-', '#'), GetLastError());

    Log().debug("Set thread affinity to mask: '{} [{}]'.", mask.to_string('-', '#'), mask.to_ullong());
    return static_cast<ProcessorAffinityMask>(prevMask);

#else
    const auto prevMask = getAvailableProcessorMask();
    const auto newMask  = Linux::affinityMaskCast(mask);
    if (sched_setaffinity(0, sizeof(newMask), &newMask) != 0) {
        Log().fatal("Failed to set thread affinity to mask: '{}' (error code: {}).", mask.to_string('-', '#'), errno);
    }

    Log().debug("Set thread affinity to mask: '{} [{}]'.", mask.to_string('-', '#'), mask.to_ullong());
    return prevMask;
#endif
}

OS::ExecutionConfig::ExecutionConfig(
    ProcessorAffinityMask processorAffinityMask, const ProcessPriority processPriority) noexcept :
    processorAffinityMask(processorAffinityMask),
    processPriority(processPriority)
{}
