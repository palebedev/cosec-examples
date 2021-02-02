add_library(asio-utils
    include/ce/asio-main.hpp
    src/asio-main.cpp
)

target_link_libraries(asio-utils PUBLIC ce::utils)

ntc_target(asio-utils ALIAS_NAME ce::asio-utils HEADER_PREFIX ce/asio-utils/)

add_library(asio-main OBJECT src/main.cpp)
add_library(ce::asio-main ALIAS asio-main)

target_link_libraries(asio-main PUBLIC ce::asio-utils)
