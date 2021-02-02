add_library(ce-utils
    include/ce/asio_ns.hpp
    include/ce/charconv.hpp
    include/ce/errno.hpp
    include/ce/executor_wrapper.hpp
    include/ce/file_descriptor.hpp
    include/ce/format.hpp
    include/ce/generator.hpp
    include/ce/get_socket.hpp
    include/ce/hex.hpp
    include/ce/io_context_signal_interrupter.hpp
    include/ce/pipe.hpp
    include/ce/socket_session.hpp
    include/ce/spawn.hpp
    include/ce/stdcoro.hpp
    include/ce/tcp_listener.hpp
    src/file_descriptor.cpp
    src/hex.cpp
    src/io_context_signal_interrupter.cpp
    src/socket_session.cpp
)

target_compile_features(ce-utils PUBLIC cxx_std_20)

target_link_libraries(ce-utils PUBLIC
    Boost::chrono
    Boost::context
    Boost::log
    Boost::thread
)

ntc_target(ce-utils ALIAS_NAME ce::utils HEADER_PREFIX ce/utils/)
