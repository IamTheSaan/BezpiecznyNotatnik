cmake_minimum_required(VERSION 3.10)
project(BezpiecznyNotatnik)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")
add_executable(main ${SOURCES})

find_package(GTest REQUIRED)
file(GLOB TEST_SOURCES "tests/*.cpp")

list(FILTER SOURCES EXCLUDE REGEX "main.cpp")
add_executable(run_tests ${TEST_SOURCES} ${SOURCES})
target_link_libraries(run_tests GTest::GTest GTest::Main)