cmake_minimum_required(VERSION 3.20)
project(OctaDock VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml Svg Widgets DBus)

qt_standard_project_setup()

set(OCTADOCK_SOURCES
    src/main.cpp
    src/DockItem.h
    src/DockItem.cpp
    src/AppLauncher.h
    src/AppLauncher.cpp
    src/DockSettings.h
    src/DockSettings.cpp
    src/DockModel.h
    src/DockModel.cpp
    src/PlatformIntegration.h
    src/PlatformIntegration.cpp
    src/SystemIconProvider.h
    src/SystemIconProvider.cpp
    src/TaskManager.h
    src/TaskManager.cpp
    src/DockWindow.h
    src/DockWindow.cpp
    resources.qrc
)

qt_add_executable(octadock
    MANUAL_FINALIZATION
    ${OCTADOCK_SOURCES}
)

target_compile_options(octadock PRIVATE -Wall -Wextra -O2)

target_link_libraries(octadock PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Qml
    Qt6::Svg
    Qt6::Widgets
    Qt6::DBus
    xcb
    xcb-shape
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK_LAYER REQUIRED gtk+-3.0 gtk-layer-shell-0)

add_executable(octadock-strut src/octadock-strut.c)
set_target_properties(octadock-strut PROPERTIES AUTOMOC OFF AUTORCC OFF AUTOUIC OFF)
target_include_directories(octadock-strut PRIVATE ${GTK_LAYER_INCLUDE_DIRS})
target_link_libraries(octadock-strut PRIVATE ${GTK_LAYER_LIBRARIES})

qt_finalize_executable(octadock)

install(TARGETS octadock octadock-strut
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
