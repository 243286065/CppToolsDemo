cmake_minimum_required (VERSION 2.8)

project(demo)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake" )
find_package(libgoPath REQUIRED)
include_directories(${LIBGO_INCLUDE})

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(src/libgo ${CMAKE_CURRENT_BINARY_DIR}/libgo_out)
link_directories(libgo_out ${CMAKE_CURRENT_BINARY_DIR}/libgo_out)

add_executable(${PROJECT_NAME} src/demo.cpp)
add_dependencies(${PROJECT_NAME} libgo_static)
target_link_libraries(${PROJECT_NAME} libgo dl pthread)