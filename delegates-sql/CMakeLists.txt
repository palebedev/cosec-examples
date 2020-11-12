add_executable(delegates-sql
    src/colordelegate.cpp
    src/colordelegate.hpp
    src/fabricsmodel.cpp
    src/fabricsmodel.hpp
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.hpp
    src/mainwindow.ui
)
target_compile_features(delegates-sql PRIVATE cxx_std_20)

target_link_libraries(delegates-sql PRIVATE Qt5::Widgets Qt5::Sql)

ntc_target(delegates-sql)
