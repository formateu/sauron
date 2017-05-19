cmake_minimum_required(VERSION 3.5.1)
project(sauron)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread system)

add_subdirectory(src)
add_subdirectory(test)
