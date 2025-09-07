#include "unit/tests/UtilsUnitTests.hpp"

#include "io/Log.hpp"
#include "utils/STL.hpp"

namespace
{

//
// CompareInclusionUnitTest
//

class CompareInclusionUnitTest : public UnitTest
{
private:
    const std::unordered_set<uint8_t> lhs, rhs;
    bool                              (*func)(uint8_t);
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
