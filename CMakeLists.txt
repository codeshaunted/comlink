cmake_minimum_required(VERSION 3.10)

project(comlink VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

# third party
add_subdirectory("third_party")

# internal
add_subdirectory("source")