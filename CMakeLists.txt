cmake_minimum_required(VERSION 3.16)

project(ToDoList VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/TaskWidget/taskwidget.cpp
    src/TaskWidget/taskwidget.h
    src/TaskWidget/taskwidget.ui
    src/Task.h
    src/Resourses.h
)

add_executable(ToDoList
    ${PROJECT_SOURCES}
    src/icons.qrc
)

target_link_libraries(ToDoList PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

install(TARGETS ToDoList
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES README.md DESTINATION .)
