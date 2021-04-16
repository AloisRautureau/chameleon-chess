cmake_minimum_required(VERSION 3.16)
project(chameleon)

set(CMAKE_CXX_STANDARD 14)

add_executable(chameleon main.cpp
        src/position.cpp src/position.h src/display.cpp src/display.h src/debug.cpp src/debug.h src/evaluation.cpp src/evaluation.h src/search.cpp src/search.h src/uci.cpp src/uci.h)
