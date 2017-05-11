cmake_minimum_required(VERSION 3.5.1)
project(sauron)

find_package(Threads REQUIRED)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES main.cpp Client.h Client.cpp Connector.h Connector.cpp Message.h MessageBuffer.cpp MessageBuffer.h)

add_executable(sauron ${SOURCE_FILES})

target_link_libraries(sauron
                      ${CMAKE_THREAD_LIBS_INIT}
                      ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                     )
