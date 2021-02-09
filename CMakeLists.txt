cmake_minimum_required(VERSION 3.16)

project(cosec-examples
    DESCRIPTION "Examples for COSEC students"
    VERSION 0.0.0
    LANGUAGES CXX ASM
)

find_package(ntc-cmake 1.1.0 REQUIRED)
include(ntc-dev-build)

find_package(Boost 1.74 REQUIRED COMPONENTS chrono container context log thread)
find_package(benchmark 1.5.0)
# Flatbuffers package provides no version info.
find_package(Flatbuffers)
find_package(Qt5 5.15 COMPONENTS LinguistTools Network Positioning Sql Svg WebEngineWidgets)

include(CheckCXXSourceCompiles)
# CMAKE_SYSTEM_PROCESSOR is not normalized between
# different operating systems.
check_cxx_source_compiles("
    #if !defined(__x86_64__)&&!defined(_M_X64)
    #error Not x86-64!
    #endif
    int main() {}
    " X86_64
)

if(Flatbuffers_FOUND)
    add_subdirectory(schema-serial)
endif()
if(benchmark_FOUND)
    if(CMAKE_SYSTEM_NAME STREQUAL Linux)
        # eventfd is required and is Linux-only.
        add_subdirectory(interprocess-copy)
    endif()
endif()

add_subdirectory(utils)
add_subdirectory(thread-pool)
add_subdirectory(asio-utils)
add_subdirectory(asio-basic)
add_subdirectory(asio-advanced)
add_subdirectory(asio-shared)
add_subdirectory(asio-stackfull-coro)
add_subdirectory(atomic)
add_subdirectory(select)
add_subdirectory(generator)

if(X86_64)
    add_subdirectory(asm-basic)
endif()

if(Qt5_FOUND)
    add_subdirectory(layouts-painting)
    add_subdirectory(item-models)
    add_subdirectory(block-scene)
    add_subdirectory(gui-progress)
    add_subdirectory(locale-resources)
    add_subdirectory(geohash)
    add_subdirectory(delegates-sql)
    add_subdirectory(treeview)
endif()

add_subdirectory(et)
