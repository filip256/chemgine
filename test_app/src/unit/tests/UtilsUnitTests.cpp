#include "unit/tests/UtilsUnitTests.hpp"

#include "io/Log.hpp"
#include "utils/Concurrency.hpp"
#include "utils/STL.hpp"

namespace
{

//
// CheckedCastUnitTest
//

template <typename SrcT, typename DstT>
class CheckedCastUnitTest : public UnitTest
{
private:
    const SrcT input;
    const bool expectSafe;

public:
    CheckedCastUnitTest(const std::string& name, const SrcT input, const bool expectSafe) noexcept;

    bool run() override final;
};

template <typename SrcT, typename DstT>
CheckedCastUnitTest<SrcT, DstT>::CheckedCastUnitTest(
    const std::string& name, const SrcT input, const bool expectSafe) noexcept :
    UnitTest(
        name + "_" + utils::getTypeName<SrcT>() + "_to_" + utils::getTypeName<DstT>() + "_" + std::to_string(input)),
    input(input),
    expectSafe(expectSafe)
{}

template <typename SrcT, typename DstT>
bool CheckedCastUnitTest<SrcT, DstT>::run()
{
    // Hide chg::fatal's logs.
    std::cerr.flush();
    std::ostringstream nullBuff;
    auto*              oldBuff = std::cerr.rdbuf(nullBuff.rdbuf());

    DstT       dst{};
    const auto castFailed = [this, &dst]() -> std::optional<std::runtime_error> {
        try {
            dst = checked_cast<DstT>(input);

        } catch (const std::runtime_error& e) {
            return e;
        }
        return std::nullopt;
    }();

    auto result = true;
    if (castFailed) {
        if (not chg::ENABLE_CHECKED_CASTS || expectSafe) {
            Log(this).error("Unexpected unsafe cast: {}.", castFailed->what());
            result = false;
        }
    }
    else if (not expectSafe) {
        Log(this).error(
            "Unexpected safe cast: {}({}) -> {}({}).",
            utils::getTypeName<SrcT>(),
            input,
            utils::getTypeName<DstT>(),
            dst);
        result = false;
    }

    std::cerr.flush();
    std::cerr.rdbuf(oldBuff);
    return result;
}

//
// NeverConcurrentUnitTest
//

class NeverConcurrentUnitTest : public UnitTest
{
private:
    const uint8_t threadCount;

public:
    NeverConcurrentUnitTest(const std::string& name, const uint8_t threadCount) noexcept;

    bool run() override final;
};

NeverConcurrentUnitTest::NeverConcurrentUnitTest(const std::string& name, const uint8_t threadCount) noexcept :
    UnitTest(name + "_t" + std::to_string(threadCount)),
    threadCount(threadCount)
{}

bool NeverConcurrentUnitTest::run()
{
    // Hide chg::fatal's logs.
    std::cerr.flush();
    std::ostringstream nullBuff;
    auto*              oldBuff = std::cerr.rdbuf(nullBuff.rdbuf());

    const auto kernel = []() {
        CHG_NEVER_CONCURRENT();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    };

    std::atomic_flag hasThrown = ATOMIC_FLAG_INIT;

    std::vector<std::thread> threads;
    threads.reserve(threadCount);
    for (uint8_t i = 0; i < threadCount; ++i) {
        threads.emplace_back([&]() {
            try {
                kernel();
            } catch (const std::runtime_error&) {
                hasThrown.test_and_set(std::memory_order_relaxed);
            }
        });
    }

    for (auto& t : threads) {
        if (t.joinable())
            t.join();
    }

    auto result = true;
    if (chg::ENABLE_CONCURRENCY_CHECKS && threadCount > 1) {
        if (not hasThrown.test(std::memory_order_relaxed)) {
            Log(this).error("Failed to detect concurrency.");
            result = false;
        }
    }
    else if (hasThrown.test(std::memory_order_relaxed)) {
        Log(this).error("False concurrency detection.");
        result = false;
    }

    std::cerr.flush();
    std::cerr.rdbuf(oldBuff);
    return result;
}

//
// CompareInclusionUnitTest
//

class CompareInclusionUnitTest : public UnitTest
{
private:
    const std::unordered_set<uint8_t> lhs, rhs;
    bool                              (*const func)(uint8_t);
    const int8_t                      expected;

public:
    CompareInclusionUnitTest(
        std::string&&                 name,
        std::unordered_set<uint8_t>&& lhs,
        std::unordered_set<uint8_t>&& rhs,
        bool                          (*func)(uint8_t),
        const int8_t                  expected) noexcept;

    bool run() override final;
};

CompareInclusionUnitTest::CompareInclusionUnitTest(
    std::string&&                 name,
    std::unordered_set<uint8_t>&& lhs,
    std::unordered_set<uint8_t>&& rhs,
    bool                          (*func)(uint8_t),
    const int8_t                  expected) noexcept :
    UnitTest(std::move(name)),
    lhs(std::move(lhs)),
    rhs(std::move(rhs)),
    func(func),
    expected(expected)
{}

bool CompareInclusionUnitTest::run()
{
    const auto actual = func ? utils::compareInclusion(lhs, rhs, func) : utils::compareInclusion(lhs, rhs);
    if (actual != expected) {
        Log(this).error("Actual result: {} differs from the expected result: {}.", actual, expected);
        return false;
    }

    const auto expectedSwap = expected == 0 || expected == utils::npos<int8_t> ? expected : -expected;
    const auto actualSwap   = func ? utils::compareInclusion(rhs, lhs, func) : utils::compareInclusion(rhs, lhs);
    if (actualSwap != expectedSwap) {
        Log(this).error("Swapped actual result: {} differs from the expected result: {}.", actualSwap, expectedSwap);
        return false;
    }

    return true;
}

}  // namespace

//
// UtilsUnitTests
//

UtilsUnitTests::UtilsUnitTests(std::string&& name, const std::regex& filter) noexcept :
    UnitTestGroup(std::move(name), filter)
{
    registerTest<CheckedCastUnitTest<uint16_t, uint8_t>>("checked_cast", 256, false);
    registerTest<CheckedCastUnitTest<int8_t, uint8_t>>("checked_cast", -1, false);
    registerTest<CheckedCastUnitTest<uint8_t, int8_t>>("checked_cast", 128, false);
    registerTest<CheckedCastUnitTest<float, int64_t>>("checked_cast", 0.5f, false);
    registerTest<CheckedCastUnitTest<int32_t, float>>("checked_cast", std::numeric_limits<int32_t>::max(), false);
    registerTest<CheckedCastUnitTest<uint32_t, float>>("checked_cast", std::numeric_limits<int32_t>::max() + 2, false);
    registerTest<CheckedCastUnitTest<double, float>>("checked_cast", 1.001 * std::numeric_limits<float>::max(), false);
    registerTest<CheckedCastUnitTest<uint16_t, uint8_t>>("checked_cast", 255, true);
    registerTest<CheckedCastUnitTest<int8_t, uint8_t>>("checked_cast", 0, true);
    registerTest<CheckedCastUnitTest<uint8_t, int8_t>>("checked_cast", 127, true);
    registerTest<CheckedCastUnitTest<float, int64_t>>("checked_cast", 1.0f, true);
    registerTest<CheckedCastUnitTest<int32_t, float>>("checked_cast", 1 << 24, true);
    registerTest<CheckedCastUnitTest<uint32_t, float>>("checked_cast", std::numeric_limits<int32_t>::max() + 1, true);
    registerTest<CheckedCastUnitTest<double, float>>("checked_cast", std::numeric_limits<float>::max(), true);

    registerTest<NeverConcurrentUnitTest>("never_concurrent", 1);
    registerTest<NeverConcurrentUnitTest>("never_concurrent", 2);

    registerTest<CompareInclusionUnitTest>(
        "compare_inclusion_same", std::unordered_set<uint8_t>{1, 2}, std::unordered_set<uint8_t>{1, 2}, nullptr, 0);
    registerTest<CompareInclusionUnitTest>(
        "compare_inclusion", std::unordered_set<uint8_t>{1, 2, 3}, std::unordered_set<uint8_t>{1, 2}, nullptr, 1);
    registerTest<CompareInclusionUnitTest>(
        "compare_inclusion_diff",
        std::unordered_set<uint8_t>{1, 2, 3},
        std::unordered_set<uint8_t>{1, 2, 4},
        nullptr,
        utils::npos<int8_t>);
    registerTest<CompareInclusionUnitTest>(
        "compare_inclusion_same_filter_both",
        std::unordered_set<uint8_t>{1, 2, 3},
        std::unordered_set<uint8_t>{1, 2, 3},
        [](const auto x) { return x != 3; },
        0);
    registerTest<CompareInclusionUnitTest>(
        "compare_inclusion_same_filter",
        std::unordered_set<uint8_t>{1, 2, 3},
        std::unordered_set<uint8_t>{1, 2},
        [](const auto x) { return x != 3; },
        0);
}
