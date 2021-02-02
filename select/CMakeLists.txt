add_executable(select
    src/async_file_descriptor.hpp
    src/composite_async_ops.hpp
    src/defer_reactor_adapter.hpp
    src/main.cpp
    src/select_reactor.cpp
    src/select_reactor.hpp
    src/task.hpp
    src/tcp_listener.cpp
    src/tcp_listener.hpp
    src/thread_pool.cpp
    src/thread_pool.hpp
)

target_link_libraries(select PRIVATE ce::utils)

ntc_target(select)
