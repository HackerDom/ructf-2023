cmake_minimum_required(VERSION 3.16)
project(server)

set(HEADERS
        include/pages_pool.hpp
        include/scheduler.hpp
        include/types.hpp
        include/interpreter.hpp
        include/server.hpp
        include/models.hpp
        include/run_id_generator.hpp
        )

set(SOURCES
        src/scheduler.cpp
        src/pages_pool.cpp
        src/interpreter.cpp
        src/server.cpp
        src/models.cpp
        src/run_id_generator.cpp
        )

add_executable(server ${HEADERS} ${SOURCES} src/main.cpp)
target_link_libraries(server common vm glog::glog)
target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(server_tests
        ${HEADERS}
        ${SOURCES}
        tests/main.cpp
        tests/pages_pool_tests.cpp
        tests/scheduler_tests.cpp
        tests/interpreter_tests.cpp
        tests/run_id_generator_tests.cpp
        )
target_include_directories(server_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(server_tests common vm glog::glog gtest)
