add_executable(item-models
    src/main.cpp
    src/mymodel.cpp
    src/mymodel.hpp
    src/testwidget.cpp
    src/testwidget.hpp
    src/testwidget.ui
)
target_compile_features(item-models PRIVATE cxx_std_20)

target_link_libraries(item-models PRIVATE Qt5::Widgets)

ntc_target(item-models)
