#include "et_vector.hpp"
#include "naive_vector.hpp"

#include <benchmark/benchmark.h>

#include <array>
#include <iostream>
#include <random>

namespace
{
    template<typename Vector>
    void BM_vector_ops(benchmark::State& state)
    {
        std::mt19937_64 prng{42};
        std::uniform_real_distribution<double> v_dist{-10.,10.};
        std::size_t n = size_t(state.range());
        std::array<Vector,3> vs;
        std::array<double,2> cs;
        auto v_gen = [&]{
            return v_dist(prng);
        };
        for(Vector& v:vs){
            v = Vector{n,boost::container::default_init};
            std::generate(v.begin(),v.end(),v_gen);
        }
        std::generate(cs.begin(),cs.end(),v_gen);
        for(auto _ : state){
            {
                asm volatile ( "#---------1-----------");
                Vector res = cs[0]*vs[0]-vs[1]/cs[1]+vs[2];
                asm volatile ( "#---------2-----------");
                benchmark::DoNotOptimize(res);
                state.PauseTiming();
            }
            state.ResumeTiming();
        }
    }
}

BENCHMARK_TEMPLATE(BM_vector_ops,naive::vector<>)->RangeMultiplier(4)->Range(1,65536);
BENCHMARK_TEMPLATE(BM_vector_ops,et::vector<>)   ->RangeMultiplier(4)->Range(1,65536);
