add_executable(thread-pool
    src/main.cpp
)
target_compile_features(thread-pool PRIVATE cxx_std_20)

target_link_libraries(thread-pool PRIVATE
    Boost::chrono Boost::system Boost::thread
)

ntc_target(thread-pool)
