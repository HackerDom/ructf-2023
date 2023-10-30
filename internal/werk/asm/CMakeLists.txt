cmake_minimum_required(VERSION 3.16)
project(asm)

set(HEADERS include/lexer.hpp)

set(SOURCES src/lexer.cpp)

add_executable(asm ${HEADERS} ${SOURCES} src/main.cpp)
target_link_libraries(asm common glog::glog)
target_include_directories(asm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(asm_shared
        SHARED
        ${SOURCES}
        ${HEADERS}
        include/extern_api.hpp
        src/extern_api.cpp
        )
set_target_properties(asm_shared PROPERTIES OUTPUT_NAME "asm")
target_include_directories(asm_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(asm_shared common_shared)

add_executable(asm_tests ${HEADERS} ${SOURCES}
        tests/main.cpp
        tests/lexer_tests.cpp
        )
target_include_directories(asm_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(asm_tests common glog::glog gtest)
