add_executable(et
    src/et_vector.hpp
    src/main.cpp
    src/naive_vector.hpp
    src/vector_base.hpp
)
target_compile_features(et PRIVATE cxx_std_20)

target_link_libraries(et PRIVATE
    Boost::container
    benchmark::benchmark_main
)

ntc_target(et)
