add_executable(block-scene
    src/block.cpp
    src/block.hpp
    src/blockmetrics.cpp
    src/blockmetrics.hpp
    src/blockpropertiesmodel.cpp
    src/blockpropertiesmodel.hpp
    src/blockscene.cpp
    src/blockscene.hpp
    src/blocktypes.cpp
    src/blocktypes.hpp
    src/blocktypesmodel.cpp
    src/blocktypesmodel.hpp
    src/connection.cpp
    src/connection.hpp
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.hpp
    src/mainwindow.ui
    src/opaquegraphicsitem.cpp
    src/opaquegraphicsitem.hpp
    src/port.cpp
    src/port.hpp
)
target_compile_features(block-scene PRIVATE cxx_std_20)

target_link_libraries(block-scene PRIVATE Qt5::Widgets)

ntc_target(block-scene)
