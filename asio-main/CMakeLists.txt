add_library(asio-main
    include/ce/main.hpp
    src/main.cpp
)

target_link_libraries(asio-main PUBLIC ce::utils)

ntc_target(asio-main ALIAS_NAME ce::asio-main HEADER_PREFIX ce/asio-main/)
