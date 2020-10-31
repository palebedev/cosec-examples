add_executable(gui-progress
    src/copywidget.cpp
    src/copywidget.hpp
    src/copywidget.ui
    src/filecopier.cpp
    src/filecopier.hpp
    src/main.cpp
)
target_compile_features(gui-progress PRIVATE cxx_std_20)

target_link_libraries(gui-progress PRIVATE Qt5::Widgets)
set_target_properties(gui-progress PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    WIN32_EXECUTABLE ON
)

ntc_target(gui-progress)
