add_executable(layouts-painting
    src/draw_widget.cpp
    src/draw_widget.hpp
    src/main.cpp
    src/widget.cpp
    src/widget.hpp
    src/widget.ui
)
target_compile_features(layouts-painting PRIVATE cxx_std_20)

target_link_libraries(layouts-painting PRIVATE Qt5::Widgets)
set_target_properties(layouts-painting PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    WIN32_EXECUTABLE ON
)

# .ui files generate headers in directories with unpredictable paths.
# Add current directory to include search paths so that we can find
# headers for promoted widgets by using global includes.
target_include_directories(layouts-painting PRIVATE src)

ntc_target(layouts-painting)
