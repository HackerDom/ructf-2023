cmake_minimum_required(VERSION 3.16)
project(common)

set(HEADERS
        include/utils/strings.hpp
        include/utils/defer.hpp
        include/utils/result.hpp
        include/utils/thread_pool.hpp
        )

set(SOURCES
        src/utils/strings.cpp
        src/utils/thread_pool.cpp
        )

add_library(common STATIC ${SOURCES} ${HEADERS})
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(common_shared SHARED ${SOURCES} ${HEADERS})
set_target_properties(common_shared PROPERTIES OUTPUT_NAME "common")
target_include_directories(common_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(common_tests
        ${SOURCES}
        ${HEADERS}
        tests/main.cpp
        tests/utils/result_tests.cpp
        tests/utils/strings_tests.cpp
        )
target_include_directories(common_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(common_tests glog::glog gtest)
