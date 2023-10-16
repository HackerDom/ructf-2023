cmake_minimum_required(VERSION 3.16)
project(vm)

set(HEADERS
        include/vm.hpp
        )

set(SOURCES
        src/vm.cpp
        )

add_library(vm ${HEADERS} ${SOURCES})
target_link_libraries(vm glog::glog)
target_include_directories(vm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(vm_tests ${HEADERS} ${SOURCES} tests/main.cpp)
target_include_directories(vm_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(vm_tests glog::glog gtest)
