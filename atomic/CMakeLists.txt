add_executable(atomic
    src/main.cpp
)
target_compile_features(atomic PRIVATE cxx_std_20)

target_link_libraries(atomic PRIVATE
    Boost::thread
)

ntc_target(atomic)
