cmake_minimum_required(VERSION 3.12)

file(GLOB_RECURSE server_sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_library(tserver_shared SHARED ${server_sources})
