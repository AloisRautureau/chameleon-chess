cmake_minimum_required(VERSION 3.16)
project(baub_chess)

set(CMAKE_CXX_STANDARD 14)

add_executable(baub_chess main.cpp
        ChessEngine/Board.h)
