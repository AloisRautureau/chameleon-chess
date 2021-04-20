cmake_minimum_required(VERSION 3.16)
project(chameleon)

set(CMAKE_CXX_STANDARD 14)

add_executable(chameleon main.cpp
        src/position.cpp src/position.h src/defs.h src/eval.cpp src/eval.h src/search.cpp src/search.h src/uci.cpp src/uci.h)
