add_executable(asio-shared
    src/chat.cpp
    src/chat.hpp
    src/chat_session.cpp
    src/chat_session.hpp
    src/main.cpp
)

target_link_libraries(asio-shared PRIVATE
    ce::asio-main
    ce::utils
)

ntc_target(asio-shared)
