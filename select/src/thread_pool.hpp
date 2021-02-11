#ifndef UUID_80373A4A_BB65_4870_A48F_A62B38052DDE
#define UUID_80373A4A_BB65_4870_A48F_A62B38052DDE

#include "task.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <vector>
#include <thread>

class thread_pool
{
public:
    thread_pool(unsigned threads = std::thread::hardware_concurrency());
    ~thread_pool();
    void join();
    void stop();
    void operator()(execution_kind kind,task_t f);
    bool running_in_this_thread() const noexcept;
private:
    std::vector<std::thread> workers_;
    std::mutex mutex_;
    std::condition_variable queue_not_empty_,no_busy_workers_;
    std::queue<task_t> tasks_;
    unsigned busy_workers_ = 0;
    // FIXME: an atomic_flag is guaranteed to always be lock-free,
    //        but it only has test() in C++20, not supported
    //        everywhere yet.
    std::atomic_bool stopping_{};
};

#endif
