add_executable(generator
    src/generator.hpp
    src/main.cpp
)
target_compile_features(generator PRIVATE cxx_std_20)

target_link_libraries(generator PRIVATE ce::utils Boost::headers)

ntc_target(generator)
