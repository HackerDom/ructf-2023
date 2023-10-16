cmake_minimum_required(VERSION 3.16)
project(werk)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pedantic")

set(BUILD_SHARED_LIBS OFF)
set(BUILD_TESTING OFF)
set(BUILD_GMOCK OFF)
add_subdirectory(googletest)
add_subdirectory(glog)

add_subdirectory(common)
add_subdirectory(asm)
add_subdirectory(vm)
add_subdirectory(server)
