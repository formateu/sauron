cmake_minimum_required(VERSION 3.5.1)
project(sauron)
set(CMAKE_CXX_STANDARD 14)

find_package(Threads REQUIRED)
#find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_subdirectory(src)
add_subdirectory (test)


##SAURON EXECUTABLE
#add_subdirectory(sauron
#        ${HEADER_FILES_SAURON}
#        ${SOURCE_FILES_SAURON})
#
#target_link_libraries(sauron
#        ${CMAKE_THREAD_LIBS_INIT}
#        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
#        yaml-cpp
#        )
enable_testing()