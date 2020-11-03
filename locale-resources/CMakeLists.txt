add_executable(locale-resources
    resources/resources.qrc
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.hpp
    src/mainwindow.ui
    src/translations.cpp
    src/translations.hpp
)
target_compile_features(locale-resources PRIVATE cxx_std_20)

target_link_libraries(locale-resources PRIVATE Qt5::Widgets)

# Since we have .qrc in sources, ntc-cmake will set
# AUTORCC target property to ON to run Qt Resource Compiler.
# See ntc-cmake for translations handling.
ntc_target(locale-resources TRANSLATIONS en ja ru)
