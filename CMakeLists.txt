cmake_minimum_required(VERSION 3.16)
project(pap_assignment3)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(header)

add_executable(pap_assignment3
        header/serial_solver.h
        src/sequential_version.cpp)
