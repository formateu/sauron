cmake_minimum_required(VERSION 3.5.1)
project(sauron)


find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Connector.cpp Connector.h Client.cpp Client.h)

add_executable(sauron ${SOURCE_FILES})

target_link_libraries(sauron ${CMAKE_THREAD_LIBS_INIT})