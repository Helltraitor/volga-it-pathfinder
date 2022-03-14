cmake_minimum_required(VERSION 3.10)

project(Volga-IT-Pathfinder
    HOMEPAGE_URL "https://github.com/Helltraitor/volga-it-pathfinder"
    LANGUAGES CXX
)

# C++ Standart
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Volga-IT-Pathfinder
    src/fairy_tail.cpp
    src/graph.cpp
    src/pathfinder.cpp
    src/main.cpp
)
