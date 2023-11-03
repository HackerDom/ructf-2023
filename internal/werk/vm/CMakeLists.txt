cmake_minimum_required(VERSION 3.16)
project(vm)

set(HEADERS
        include/vm.hpp
        include/arch/opcodes.hpp
        include/arch/types.hpp
        include/arch/registers.hpp
        include/arch/constants.hpp
        include/arch/instructions.hpp
        )

set(SOURCES
        src/vm.cpp
        src/arch/opcodes.cpp
        src/arch/registers.cpp
        src/arch/instructions.cpp
        )

add_library(vm STATIC ${HEADERS} ${SOURCES})
target_link_libraries(vm common glog::glog)
target_include_directories(vm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(vm_exe
        ${HEADERS}
        ${SOURCES}
        src/main.cpp
        )
target_include_directories(vm_exe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(vm_exe PROPERTIES OUTPUT_NAME "vm")
target_link_libraries(vm_exe common glog::glog)

add_executable(vm_tests
        ${HEADERS}
        ${SOURCES}
        tests/main.cpp
        tests/instructions_tests.cpp
        tests/vm_tests.cpp
        )
target_include_directories(vm_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(vm_tests common glog::glog gtest)
