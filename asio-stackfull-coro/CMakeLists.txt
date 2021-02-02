add_executable(asio-stackfull-coro
    src/main.cpp
)

target_link_libraries(asio-stackfull-coro PRIVATE ce::asio-main)

ntc_target(asio-stackfull-coro)
