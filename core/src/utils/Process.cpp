#include "utils/Process.hpp"

#include "io/Log.hpp"
#include "utils/Build.hpp"
#include "utils/Casts.hpp"

#include <array>

#ifdef CHG_BUILD_WINDOWS
    #include <Windows.h>
#endif

OS::ProcessPriority OS::getCurrentProcessPriority()
{
#ifdef CHG_BUILD_WINDOWS
    const auto priority = GetPriorityClass(GetCurrentProcess());
    if (not priority)
        Log().fatal("Failed to get process priority (error code: {0}).", GetLastError());

    return static_cast<ProcessPriority>(priority);
#else
    return ProcessPriority::NORMAL_PRIORITY_CLASS;
#endif
}

void OS::setCurrentProcessPriority(const OS::ProcessPriority priority)
{
#ifdef CHG_BUILD_WINDOWS
    if (not SetPriorityClass(GetCurrentProcess(), static_cast<DWORD>(priority)))
        Log().fatal("Failed to set process priority: {0} (error code: {1}).", underlying_cast(priority), GetLastError());

    Log().debug("Set process priority: {0}.", underlying_cast(priority));
#endif
}

#ifdef CHG_BUILD_WINDOWS

namespace
{

class ProcessorInfo
{
private:
    OS::ProcessorIndex                                   count;
    std::array<SYSTEM_LOGICAL_PROCESSOR_INFORMATION, 64> info;

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
        Log().fatal("Failed to get processor information (error code: {0}).", GetLastError());

    count = static_cast<OS::ProcessorIndex>(length / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
}

OS::ProcessorIndex ProcessorInfo::getCount() { return processorInfo.count; }

SYSTEM_LOGICAL_PROCESSOR_INFORMATION ProcessorInfo::getInfo(const OS::ProcessorIndex idx)
{
    if (idx >= processorInfo.count)
        Log().fatal("Invalid processor index: {0}, system only has: {1} processors.", idx, processorInfo.count);

    return processorInfo.info[idx];
}

}  // namespace

#endif

OS::ProcessorAffinityMask OS::getAvailableProcessorMask()
{
#ifdef CHG_BUILD_WINDOWS
    DWORD_PTR processAffinityMask, systemAffinityMask;
    if (not GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask, &systemAffinityMask))
        Log().fatal("Failed to get process affinity mask (error code: {0}).", GetLastError());

    return static_cast<ProcessorAffinityMask>(processAffinityMask);
#else
    return static_cast<ProcessorAffinityMask>(0);
#endif
}

OS::ProcessorAffinityMask OS::getAvailablePhysicalProcessorMask()
{
#ifdef CHG_BUILD_WINDOWS
    static const auto     availableProcessors = getAvailableProcessorMask();
    ProcessorAffinityMask physicalProcessors  = {0};

    for (uint8_t i = 0; i < sizeof(DWORD_PTR) * 8; ++i) {
        if (availableProcessors.test(i) && ProcessorInfo::getInfo(i).Relationship == RelationProcessorCore)
            physicalProcessors.set(i);
    }

    return static_cast<ProcessorAffinityMask>(physicalProcessors);
#else
    return static_cast<ProcessorAffinityMask>(0);
#endif
}

OS::ProcessorAffinityMask OS::getLastAvailablePhysicalProcessorMask()
{
#ifdef CHG_BUILD_WINDOWS
    static const auto     availableProcessors = getAvailableProcessorMask();
    ProcessorAffinityMask selectedProcessor   = {0};

    for (uint8_t i = sizeof(DWORD_PTR) * 8; i-- > 0;) {
        if (availableProcessors.test(i) && ProcessorInfo::getInfo(i).Relationship == RelationProcessorCore) {
            selectedProcessor.set(i);
            return selectedProcessor;
        }
    }

    Log().fatal("Found no physical processor.");
    CHG_UNREACHABLE();
#else
    return static_cast<ProcessorAffinityMask>(0);
#endif
}

OS::ProcessorAffinityMask OS::setCurrentThreadProcessorAffinity(const ProcessorAffinityMask mask)
{
#ifdef CHG_BUILD_WINDOWS
    const auto prevMask = SetThreadAffinityMask(GetCurrentThread(), static_cast<DWORD_PTR>(mask.to_ullong()));
    if (not prevMask)
        Log().fatal(
            "Failed to set thread affinity to mask: '{0}' (error code: {1}).", mask.to_string('-', '#'), GetLastError());

    Log().debug("Set thread affinity to mask: '{0} [{1}]'.", mask.to_string('-', '#'), mask.to_ullong());
    return static_cast<ProcessorAffinityMask>(prevMask);
#else
    return static_cast<ProcessorAffinityMask>(0);
#endif
}

OS::ExecutionConfig::ExecutionConfig(
    ProcessorAffinityMask processorAffinityMask, const ProcessPriority processPriority) noexcept :
    processorAffinityMask(processorAffinityMask),
    processPriority(processPriority)
{}
