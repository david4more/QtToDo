cmake_minimum_required(VERSION 3.16)

project(Thinker VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

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

add_executable(Thinker
    ${PROJECT_SOURCES}
    src/files.qrc
)

target_link_libraries(Thinker PRIVATE Qt6::Widgets Qt6::Multimedia)

install(TARGETS Thinker
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES README.md DESTINATION .)
