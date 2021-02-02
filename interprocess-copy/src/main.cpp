// This example deliberately uses raw system calls instead of
// existing library implementations.

#include <config.hpp>

#include "event.hpp"
#include "fork.hpp"
#if HAVE_KNEM
#include "knem.hpp"
#endif
#include "memfd.hpp"

#include <ce/pipe.hpp>

#include <benchmark/benchmark.h>

#include <iostream>
#include <optional>

#include <sys/uio.h>

namespace
{
    constexpr static std::size_t page_size = 4096,
                                 data_size = 1<<30;
    std::vector<std::byte> data(data_size);
    std::optional<class fork> fork_;
    // These might throw on creation which will cause the process to terminate().
    // This is fine for a benchmark, otherwise we could use optionals as for the
    // mapping to move actual construction to main().
    event event_to_child,event_to_parent;
    memfd shmem{data_size};
    std::optional<memfd::mapping> mapping;
    ce::pipe data_pipe;
#if HAVE_KNEM
    knem knem_file;
    knem_cookie_t parent_cookie;
#endif

    constexpr std::uint64_t quit_op          = 0x100000000;
    constexpr std::uint64_t shared_memory_op = 0x200000000;
    constexpr std::uint64_t pipe_op          = 0x300000000;
    constexpr std::uint64_t vm_write_op      = 0x400000000;
#if HAVE_KNEM
    constexpr std::uint64_t knem_op          = 0x500000000;
#endif
    constexpr std::uint64_t op_mask          = 0xf00000000;
    constexpr std::uint64_t n_mask           = 0x0ffffffff;

    // If the data to be sent to the other process can be created
    // and processed already in a shared memory segment, the cost
    // of "sending" it is the cost of synchronization primitive,
    // which is not useful to compare against other methods.
    // In this case we consider the scenario where copies happen
    // at both sides.

    void shared_memory(benchmark::State& state)
    {
        auto n = std::uint64_t(state.range());
        for(auto _ : state){
            std::copy(data.data(),data.data()+n,mapping->data());
            event_to_child.write(shared_memory_op|n);
            event_to_parent.read();
        }
    }

    void shared_child(uint64_t n)
    {
        std::copy(mapping->data(),mapping->data()+n,data.data());
        event_to_parent.write();
    }

    void pipe_(benchmark::State& state)
    {
        auto n = std::size_t(state.range());
        for(auto _ : state){
            event_to_child.write(pipe_op|n);
            data_pipe[1].write_as_single({data.data(),n});
            event_to_parent.read();
        }
    }

    void pipe_child(uint64_t n)
    {
        data_pipe[0].read_as_chunks({data.data(),std::size_t(n)});
        event_to_parent.write();
    }

    void vm_write(benchmark::State& state)
    {
        auto n = std::size_t(state.range());
        for(auto _ : state){
            struct iovec iv = {data.data(),n};
            ssize_t ret = process_vm_writev(fork_->child_pid(),&iv,1,&iv,1,0);
            ce::throw_errno_if_negative(ret,"process_vm_write");
            if(std::size_t(ret)!=n)
                throw std::runtime_error("invalid process_vm_write count");
            event_to_child.write(vm_write_op|n);
            event_to_parent.read();
        }
    }

    void vm_write_child(uint64_t /*n*/)
    {
        event_to_parent.write();
    }

#if HAVE_KNEM
    void knem_(benchmark::State& state)
    {
        auto n = std::uint64_t(state.range());
        for(auto _ : state){
            event_to_child.write(knem_op|n);
            event_to_parent.read();
        }
    }

    void knem_child(uint64_t n)
    {
        knem_file.copy_from(parent_cookie,0,{data.data(),std::size_t(n)});
        event_to_parent.write();
    }
#endif

    void benchmark_args(benchmark::internal::Benchmark* b)
    {
        b->Arg(8)
         ->Arg(page_size/4)
         ->Arg(page_size/2)
         ->Arg(page_size)
         ->Arg(page_size*2)
         ->Arg(page_size*4)
         ->Arg(1<<20)
         ->Arg(1<<24)
         ->Arg(data_size)
         ->UseRealTime();
    }
}

BENCHMARK(shared_memory)->Apply(benchmark_args);
BENCHMARK(pipe_)        ->Apply(benchmark_args);
BENCHMARK(vm_write)     ->Apply(benchmark_args);
#if HAVE_KNEM
BENCHMARK(knem_)        ->Apply(benchmark_args);
#endif

int main(int argc,char* argv[])
{
    try{
        // Fork
        fork_.emplace();
        // After-fork mapping
        mapping = shmem.map(0,data_size);
        if(*fork_){
            // Parent code
#if HAVE_KNEM
            auto region = knem_file.create_read_region(data);
            data_pipe[1].write_as_single({reinterpret_cast<const std::byte*>(&region.cookie()),
                                          sizeof(knem_cookie_t)});
#endif
            // Benchmark
            benchmark::Initialize(&argc,argv);
            if(!benchmark::ReportUnrecognizedArguments(argc,argv))
                // Exceptions are caught within this function.
                benchmark::RunSpecifiedBenchmarks();
            event_to_child.write(quit_op);
        }else{
            // Child code
#if HAVE_KNEM
            data_pipe[0].read_as_single({reinterpret_cast<std::byte*>(&parent_cookie),
                                         sizeof parent_cookie});
#endif
            // Benchmark counterpart
            for(;;){
                uint64_t op = event_to_child.read();
                uint64_t n = op&n_mask;
                switch(op&op_mask){
                    case quit_op:
                        return 0;
                    case shared_memory_op:
                        shared_child(n);
                        break;
                    case pipe_op:
                        pipe_child(n);
                        break;
                    case vm_write_op:
                        vm_write_child(n);
#if HAVE_KNEM
                        break;
                    case knem_op:
                        knem_child(n);
#endif
                }
            }
        }
    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
        return 1;
    }
}
