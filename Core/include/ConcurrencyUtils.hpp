#pragma once

#include <mutex>
#include <atomic>
#include <source_location>
#include <format>
#include <sstream>

//
// MUTEX_LOCK
//

#define CHG_MUTEX_LOCK()                \
	static std::mutex __mtx;            \
    std::lock_guard __mutex_lock(__mtx)

//
// NEVER_CONCURRENT
//

namespace details
{

class NoConcurrencyGuard
{
    std::atomic_flag& flag;

public:
    NoConcurrencyGuard(
        std::atomic_flag& flag,
        const std::source_location& location = std::source_location::current()
    );
    ~NoConcurrencyGuard();

};

} // namespace details

// #define CHG_DISABLE_CONCURRENCY_CHECKS

#ifdef CHG_DISABLE_CONCURRENCY_CHECKS
    #define CHG_NEVER_CONCURRENT()
#else
    #define CHG_NEVER_CONCURRENT()                         \
        static std::atomic_flag __flag = ATOMIC_FLAG_INIT; \
        details::NoConcurrencyGuard __guard(__flag) 
#endif

//
// std::thread::id formatter
//

// TODO: Remove in C++23.

template <>
struct std::formatter<std::thread::id>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const std::thread::id& id, FormatContext& ctx) const
    {
        std::ostringstream os;
        os << id;
        return std::format_to(ctx.out(), "{}", os.str());
    }
};
