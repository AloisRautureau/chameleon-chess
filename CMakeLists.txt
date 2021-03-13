cmake_minimum_required(VERSION 3.16)
project(chameleon)

set(CMAKE_CXX_STANDARD 14)

add_executable(chameleon main.cpp
        ChessEngine/board_representation.cpp ChessEngine/board_representation.h ChessEngine/display.cpp ChessEngine/display.h ChessEngine/debug.cpp ChessEngine/debug.h ChessEngine/evaluation.cpp ChessEngine/evaluation.h ChessEngine/search.cpp ChessEngine/search.h)
