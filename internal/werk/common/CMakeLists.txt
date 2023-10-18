cmake_minimum_required(VERSION 3.16)
project(common)

set(HEADERS
        include/utils/strings.hpp
        include/utils/defer.hpp
        include/utils/thread_pool.hpp
        include/arch/opcodes.hpp
        include/arch/types.hpp
        include/arch/registers.hpp
        include/arch/constants.hpp
        )

set(SOURCES
        src/utils/strings.cpp
        src/arch/opcodes.cpp
        src/arch/registers.cpp
        )

add_library(common ${SOURCES} ${HEADERS})
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(common_tests
        ${SOURCES}
        ${HEADERS}
        tests/main.cpp
        tests/arch/opcodes_tests.cpp
        tests/utils/strings_tests.cpp
        tests/arch/registers_tests.cpp
        )
target_include_directories(common_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(common_tests glog::glog gtest)
