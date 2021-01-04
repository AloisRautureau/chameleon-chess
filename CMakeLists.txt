cmake_minimum_required(VERSION 3.17)
project(baub_chess)

set(CMAKE_CXX_STANDARD 14)

add_executable(baub_chess main.cpp
        ChessEngine/BoardRepresentation.cpp ChessEngine/BoardRepresentation.h
        ChessEngine/Engine.cpp ChessEngine/Engine.h
        ChessEngine/Utility.h ChessEngine/Data.h)

add_subdirectory(googletest)