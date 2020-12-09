cmake_minimum_required(VERSION 3.16)

project(cosec-examples
    DESCRIPTION "Examples for COSEC students"
    VERSION 0.0.0
    LANGUAGES CXX
)

find_package(ntc-cmake 1.1.0 REQUIRED)
include(ntc-dev-build)

find_package(Boost 1.74 REQUIRED COMPONENTS chrono container log system thread)
find_package(benchmark 1.5.0)
# Flatbuffers package provides no version info.
find_package(Flatbuffers)
find_package(Qt5 5.15 COMPONENTS LinguistTools Network Positioning Sql Svg WebEngineWidgets)

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
add_subdirectory(asio-main)
add_subdirectory(asio-basic)
add_subdirectory(asio-advanced)
add_subdirectory(asio-shared)

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
