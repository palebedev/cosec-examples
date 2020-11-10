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
        auto v_gen = [&]{
            return v_dist(prng);
        };
        std::size_t n = size_t(state.range());
        Vector u,v,w;
        double a,b;
        for(Vector* v:{&u,&v,&w}){
            *v = Vector{n,boost::container::default_init};
            std::generate(v->begin(),v->end(),v_gen);
        }
        for(double* x:{&a,&b})
            *x = v_gen();
        for(auto _ : state){
            {
                asm volatile("#---------1-----------");
                Vector res = a*u-v/b+w;
                asm volatile("#---------2-----------");
                benchmark::DoNotOptimize(res);
                state.PauseTiming();
            }
            state.ResumeTiming();
        }
    }
}

BENCHMARK_TEMPLATE(BM_vector_ops,naive::vector<double>)->RangeMultiplier(4)->Range(1,65536);
BENCHMARK_TEMPLATE(BM_vector_ops,et::vector<double>)   ->RangeMultiplier(4)->Range(1,65536);
