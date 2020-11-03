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

# .ui files generate headers in directories with unpredictable paths.
# Add current directory to include search paths so that we can find
# headers for promoted widgets by using global includes.
target_include_directories(layouts-painting PRIVATE src)

# This sets the following target properties automatically,
# which is needed for proper build:
# - AUTOMOC ON (because we link to Qt5::*) - run Qt Meta Object Compiler.
# - AUTOUIC ON (because .ui files are in sources) - run Qt UI Compiler.
# - WIN32_EXECUTABLE ON (because we link to Qt5::Widgets) - don't create
#   a new console for the application when run on Windows.
ntc_target(layouts-painting)
