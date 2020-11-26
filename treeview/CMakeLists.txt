add_executable(treeview
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.hpp
    src/mainwindow.ui
    src/notes.cpp
    src/notes.hpp
    src/notesmodel.cpp
    src/notesmodel.hpp
)
target_compile_features(treeview PRIVATE cxx_std_20)

target_link_libraries(treeview PRIVATE Qt5::Widgets)

ntc_target(treeview)
