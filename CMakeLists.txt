cmake_minimum_required(VERSION 3.5.1)
project(sauron)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Connector.cpp Connector.h)

add_executable(sauron ${SOURCE_FILES})
