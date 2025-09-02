#pragma once

#include <atomic>
#include <format>
#include <mutex>
#include <source_location>
#include <sstream>
#include <thread>

//
// MUTEX_LOCK
//

#define CHG_MUTEX_LOCK()                  \
    static std::mutex __mtx;              \
    std::lock_guard   __mutex_lock(__mtx)

//
// NEVER_CONCURRENT
//

namespace details
{

class NoConcurrencyGuard
{
    std::atomic_flag& flag;

public:
    NoConcurrencyGuard(std::atomic_flag& flag, const std::source_location& location = std::source_location::current());
    ~NoConcurrencyGuard();
};

}  // namespace details

#ifdef CHG_ENABLE_CONCURRENCY_CHECKS
    #define CHG_NEVER_CONCURRENT()                             \
        static std::atomic_flag     __flag = ATOMIC_FLAG_INIT; \
        details::NoConcurrencyGuard __guard(__flag)
#else
    #define CHG_NEVER_CONCURRENT()
#endif

//
// std::thread::id formatter
//

// TODO: Remove in C++23.

template <>
struct std::formatter<std::thread::id>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const std::thread::id& id, FormatContext& ctx) const
    {
        std::ostringstream os;
        os << id;
        return std::format_to(ctx.out(), "{}", os.str());
    }
};
