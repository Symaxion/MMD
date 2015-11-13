cmake_minimum_required(VERSION 2.8.11)
project(spiral)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Werror")
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)

add_executable(spiral
    ImageViewer.cpp
    ImageWidget.cpp
    MainWindow.cpp
    Spiralify.cpp

    main.cpp
)

target_link_libraries(spiral
    Qt5::Widgets
)

