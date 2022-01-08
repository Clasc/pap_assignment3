cmake_minimum_required(VERSION 3.16)

project(pap_assignment3)

set(CMAKE_CXX_STANDARD 14)


include_directories(.)
include_directories(src)

IF (WIN32)
include_directories(${OpenCL_ROOT})
ELSE()
find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})
ENDIF()

add_executable(pap_assignment3
        src/main.cpp
)

target_link_libraries(pap_assignment3 OpenCL)
