cmake_minimum_required(VERSION 3.16)
project(server)

set(HEADERS )

set(SOURCES )

add_executable(server ${HEADERS} ${SOURCES} src/main.cpp)
target_link_libraries(server common glog::glog)
target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(server_tests ${HEADERS} ${SOURCES} tests/main.cpp)
target_include_directories(server_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(server_tests common glog::glog gtest)
