add_executable(anf
    src/anf.cpp
    src/anf.hpp
    src/main.cpp
)

if(HAVE_AVX2)
    target_sources(anf PRIVATE
        src/anf_avx2.cpp
        src/anf_avx2.hpp
    )
endif()

target_link_libraries(anf PRIVATE
    ce::utils
    benchmark::benchmark_main
)

ntc_target(anf)
