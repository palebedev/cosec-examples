add_executable(geohash
    src/main.cpp
    src/mainwidget.cpp
    src/mainwidget.hpp
    src/mainwidget.ui
)
target_compile_features(geohash PRIVATE cxx_std_20)

target_link_libraries(geohash PRIVATE
    Qt5::Widgets
    Qt5::Network
    Qt5::Positioning
    Qt5::WebEngineWidgets
)

ntc_target(geohash)
