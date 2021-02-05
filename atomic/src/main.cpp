#include <boost/asio/static_thread_pool.hpp>

#include <atomic>
#include <chrono>
#include <iostream>
#include <random>
#include <thread>

int main()
{
    using namespace std::chrono_literals;

    // Perform work using thread_count threads with work_per_thread
    // work items per thread each taking between min_work_time and max_work_time.
    // Update progress display every update_tick.
    unsigned thread_count = std::thread::hardware_concurrency();
    constexpr std::size_t work_per_thread = 500;
    constexpr auto min_work_time = 10ms,max_work_time = 20ms;
    constexpr auto update_tick = 500ms;

    boost::asio::static_thread_pool pool{thread_count};
    // On every update a cache line holding processed counter will likely
    // move between cpus. We don't want it to "falsely" share with any other
    // objects so we give it alignmet the size of the cache line.
    // std::hardware_dstructive_interference_size is a portable way to get
    // this size, but it's not supported in most compilers so we use a constant.
    constexpr std::size_t cacheline_alignment = 64;
    alignas(cacheline_alignment) std::atomic<std::uint64_t> processed{};
    // Same for number of threads still working.
    alignas(cacheline_alignment) std::atomic<unsigned> threads_left = thread_count;
    // Even if we've made sure a cache line holding the counter doesn't make
    // any other data move between cpu caches, it would still generate a lot
    // of cache coherency traffic we update it too fast. Our progress is an
    // estimate anyway, so we calculate about how many other work items we
    // need to actually update progress. In real examples where work item time
    // is not known beforehand this constant is chosen empirically or a check
    // against some cheap clock is made whether enough time has passed to issue
    // an update.
    constexpr std::size_t update_every = update_tick/((min_work_time+max_work_time)/2);

    for(std::size_t i=0;i<thread_count;++i)
        boost::asio::execution::execute(pool.get_executor(),[&]() mutable {
            std::default_random_engine prng{std::random_device{}()};
            std::uniform_int_distribution<decltype(min_work_time)::rep> work_dist
                {min_work_time.count(),max_work_time.count()};
            std::uint64_t done = 0;
            for(std::size_t j=0;j<work_per_thread;++j){
                // Simulate work.
                std::this_thread::sleep_for(std::chrono::milliseconds{
                    work_dist(prng)});
                // If we've done enough work to update the counter, do it.
                if(!(++done%update_every))
                    // Our counter is an estimate, so no ordering is required.
                    processed.fetch_add(update_every,std::memory_order::relaxed);
            }
            // Add the rest of the work done to counter.
            processed.fetch_add(done%update_every,std::memory_order::relaxed);
            // Notify that this thread is done. Release semantics ensures any
            // other useful work we might have done is visible to the main
            // thread once it reads a zero from here with acquire semantics.
            threads_left.fetch_sub(1,std::memory_order::release);
        });
    // Loop until work is done.
    while(threads_left.load(std::memory_order::acquire)){
        std::cout << "Processed: "
        // Relaxed load, this is just an estimate.
                  << processed.load(std::memory_order::relaxed) << '\n';
        std::this_thread::sleep_for(update_tick);
        // We don't immediately notice when the threads are done as we check
        // only every update_tick. This is a compromise that allows us to
        // avoid all use of mutexes.
    }
    std::cout << "Done, total processed: "
    // Relaxed load, at this point we're guaranteed to have no concurrent access.
              << processed.load(std::memory_order::relaxed) << " = "
              << thread_count << " threads * " << work_per_thread << " work items.\n";
}

