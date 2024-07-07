cmake_minimum_required(VERSION 3.28)
project(CASM_main)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
set(CMAKE_EXE_LINKER_FLAGS -static)
add_executable(CASM_main
        hint.hpp
        main.cpp)
