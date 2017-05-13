cmake_minimum_required(VERSION 3.5.1)
project(sauron)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

find_package(Threads REQUIRED)

add_subdirectory(src)
add_subdirectory(test)
