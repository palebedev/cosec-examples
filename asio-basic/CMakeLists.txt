add_executable(asio-basic
    src/main.cpp
)

target_link_libraries(asio-basic PRIVATE ce::asio-main)

ntc_target(asio-basic)
