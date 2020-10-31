add_library(ce-utils
    include/ce/asio_ns.hpp
    include/ce/charconv.hpp
    include/ce/format.hpp
    include/ce/io_context_signal_interrupter.hpp
    include/ce/socket_session.hpp
    include/ce/tcp_listener.hpp
    src/io_context_signal_interrupter.cpp
    src/socket_session.cpp
    src/tcp_listener.cpp
)

target_compile_features(ce-utils PUBLIC cxx_std_20)

target_link_libraries(ce-utils PUBLIC
    Boost::chrono
    Boost::log
    Boost::system
    Boost::thread
)

ntc_target(ce-utils ALIAS_NAME ce::utils HEADER_PREFIX ce/utils/)
