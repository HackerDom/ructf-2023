cmake_minimum_required(VERSION 3.16)
project(server)

set(HEADERS
        include/pages_pool.hpp
        include/scheduler.hpp
        include/types.hpp
        include/interpreter.hpp
        include/server.hpp
        include/models.hpp
        include/vd_generator.hpp
        include/run.hpp
        )

set(SOURCES
        src/scheduler.cpp
        src/pages_pool.cpp
        src/interpreter.cpp
        src/server.cpp
        src/models.cpp
        src/vd_generator.cpp
        src/run.cpp
        )

add_executable(interpreter_server ${HEADERS} ${SOURCES} src/main.cpp)
target_link_libraries(interpreter_server common vm glog::glog)
target_include_directories(interpreter_server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(interpreter_server_tests
        ${HEADERS}
        ${SOURCES}
        tests/main.cpp
        tests/pages_pool_tests.cpp
        tests/scheduler_tests.cpp
        tests/interpreter_tests.cpp
        tests/vd_generator_tests.cpp
        tests/run_tests.cpp
        )
target_include_directories(interpreter_server_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(interpreter_server_tests common vm glog::glog gtest)
