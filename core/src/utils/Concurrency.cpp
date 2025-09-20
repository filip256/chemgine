#include "utils/Concurrency.hpp"

#include "utils/Exception.hpp"
#include "utils/Path.hpp"

namespace details
{

NoConcurrencyGuard::NoConcurrencyGuard(std::atomic_flag& flag, const std::source_location& location) :
    flag(flag)
{
    if (flag.test_and_set(std::memory_order_acquire)) {
        auto pathStr = utils::getRelativePathToProjectRoot(location.file_name());
        utils::normalizePath(pathStr);
        chg::fatal(
            "Concurrency detected in NEVER_CONCURRENT context, at: '{}:{}', thread id: {}.",
            pathStr,
            location.line(),
            std::this_thread::get_id());
    }
}

NoConcurrencyGuard::~NoConcurrencyGuard() { flag.clear(std::memory_order_release); }

}  // namespace details
