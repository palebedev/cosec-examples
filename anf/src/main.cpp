#include "anf.hpp"

#if defined(__AVX2__)
#include "anf_avx2.hpp"
#endif

#include <ce/random.hpp>

#include <benchmark/benchmark.h>

namespace
{
    using namespace ce;

    template<void (*Impl)(std::span<std::byte>)>
    void test_anf(benchmark::State& state)
    {
        auto data = random_bytes(std::size_t(state.range()));
        for(auto _:state){
            Impl(data);
            benchmark::DoNotOptimize(data.data());
        }
        state.counters["byte/s"] = benchmark::Counter{double(state.range()),
                                                      benchmark::Counter::kIsIterationInvariantRate};
    }
}

BENCHMARK_TEMPLATE(test_anf,ce::anf)->Arg(32)->Arg(4096)->Arg(1<<18)->Arg(1<<21)->Arg(1<<27);
#if defined(__AVX2__)
BENCHMARK_TEMPLATE(test_anf,ce::anf_avx2)->Arg(32)->Arg(4096)->Arg(1<<18)->Arg(1<<21)->Arg(1<<27);
#endif
