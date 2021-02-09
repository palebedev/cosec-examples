add_executable(asm-basic
    src/add.hpp
    src/add.S
    src/main.cpp
)

ntc_target(asm-basic)
