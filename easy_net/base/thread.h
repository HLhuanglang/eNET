#include <cstddef>
#include <thread>
#ifdef _WIN32
#    include <processthreadsapi.h>
#elif defined(__linux__)
#    include <sys/syscall.h>
#    include <unistd.h>
#else
#    include <thread>
#endif

namespace EasyNet {
inline size_t GetCurrentThreadId() {
#ifdef _WIN32
    return static_cast<long long>(::GetCurrentThreadId());
#elif defined(__linux__)
    return static_cast<long long>(syscall(SYS_gettid));
#else // Default to standard C++11 (other Unix)
    return static_cast<size_t>(std::hash<std::thread::id>()(std::this_thread::get_id()));
#endif
}
} // namespace EasyNet