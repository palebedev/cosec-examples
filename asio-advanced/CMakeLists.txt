add_executable(asio-advanced
    src/main.cpp
)

target_link_libraries(asio-advanced PRIVATE ce::asio-main)

ntc_target(asio-advanced)
