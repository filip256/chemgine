#include "unit/tests/ProcessUnitTests.hpp"

#include "io/Log.hpp"
#include "utils/Build.hpp"
#include "utils/Casts.hpp"
#include "utils/Process.hpp"

namespace
{

//
// PriorityUnitTest
//

class PriorityUnitTest : public UnitTest
{
private:
    const OS::ProcessPriority priority;

public:
    PriorityUnitTest(std::string&& name, const OS::ProcessPriority priority) noexcept;
    using UnitTest::UnitTest;

    bool run() override final;
};

PriorityUnitTest::PriorityUnitTest(std::string&& name, const OS::ProcessPriority priority) noexcept :
    UnitTest(std::move(name)),
    priority(priority)
{}

bool PriorityUnitTest::run()
{
    const auto prev = OS::getCurrentProcessPriority();

    OS::setCurrentProcessPriority(priority);
    auto actual = OS::getCurrentProcessPriority();
    if (priority != actual) {
        Log(this).error(
            "Failed to set the process priority to '{}' (actual: '{}').",
            underlying_cast(priority),
            underlying_cast(actual));
        return false;
    }

    OS::setCurrentProcessPriority(prev);
    actual = OS::getCurrentProcessPriority();
    if (prev != OS::getCurrentProcessPriority()) {
        Log(this).error(
            "Failed to reset the process priority to '{}' (actual: '{}').",
            underlying_cast(prev),
            underlying_cast(actual));
        return false;
    }

    return true;
}

//
// ProcessorAffinityUnitTest
//

class ProcessorAffinityUnitTest : public UnitTest
{
public:
    using UnitTest::UnitTest;

    bool run() override final;
};

bool ProcessorAffinityUnitTest::run()
{
    const auto availableMask = OS::getAvailableProcessorMask();
    if (availableMask.none()) {
        Log(this).error("Empty available processor mask.");
        return false;
    }

    const auto physicalMask = OS::getAvailablePhysicalProcessorMask();
    if (availableMask.none()) {
        Log(this).error("Empty available physical processor mask.");
        return false;
    }

    const auto prev = OS::setCurrentThreadProcessorAffinity(physicalMask);
    if (prev != availableMask) {
        Log(this).error(
            "Unexpected previous processor mask: '{}' (expected: '{}').",
            prev.to_string('-', '#'),
            availableMask.to_string('-', '#'));
        return false;
    }

    // WinAPI doesn't provide a direct method to get the current thread's affinity mask.
    // The only way to get it is by setting another affinity mask and listen to the returned mask.
    // We set the same mask a second time to verify the first update worked.
    auto actual = OS::setCurrentThreadProcessorAffinity(physicalMask);
    if (physicalMask != actual) {
        Log(this).error(
            "Failed to set processor mask: '{}' (actual: '{}').",
            physicalMask.to_string('-', '#'),
            actual.to_string('-', '#'));
        return false;
    }

    OS::setCurrentThreadProcessorAffinity(availableMask);
    actual = OS::setCurrentThreadProcessorAffinity(availableMask);
    if (availableMask != actual) {
        Log(this).error(
            "Failed to reset processor mask: '{}' (actual: '{}').",
            availableMask.to_string('-', '#'),
            actual.to_string('-', '#'));
        return false;
    }

    return true;
}

}  // namespace

//
// ProcessUnitTests
//

ProcessUnitTests::ProcessUnitTests(std::string&& name, const std::regex& filter) noexcept :
    UnitTestGroup(std::move(name), filter)
{
    registerTest<PriorityUnitTest>("lower_priority", OS::ProcessPriority::BELOW_NORMAL_PRIORITY_CLASS);
    registerTest<PriorityUnitTest>("higher_priority", OS::ProcessPriority::HIGH_PRIORITY_CLASS);

    // Without elevated privilege, Windows will silently set HIGH_PRIORITY_CLASS instead of REALTIME_PRIORITY_CLASS.
    CHG_LINUX_ONLY(registerTest<PriorityUnitTest>("highest_priority", OS::ProcessPriority::REALTIME_PRIORITY_CLASS));

    registerTest<ProcessorAffinityUnitTest>("processor_affinity_mask");
}
