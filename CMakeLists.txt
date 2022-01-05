cmake_minimum_required(VERSION 3.16)
project(pap_assignment3)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(header)
find_package(OpenCL REQUIRED)


add_executable(pap_assignment3
        src/sequential_version.cpp
        src/parallel_version.cpp
        )

target_link_libraries(pap_assignment3 OpenCL_LIBRARY OpenCL_INCLUDE_DIR)
