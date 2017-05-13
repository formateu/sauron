add_definitions(-DBOOST_TEST_DYN_LINK)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/test/common)

#SAURON UNIT TESTS EXECUTABLE
set(SOURCE_FILES_SAURON_UNIT_TEST
        UnitTestMain.cpp
        TestMessageBuffer.cpp
        TestClient.cpp
        TestInternetConnector.cpp
        TestTimer.cpp
        )

add_executable(sauron_unit_test
        ${SOURCE_FILES_SAURON_UNIT_TEST})

target_link_libraries(sauron_unit_test
        ${CMAKE_THREAD_LIBS_INIT}
        sauron_lib
        sauron_tests_common
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
        yaml-cpp
        )

add_test(sauron_unit_test sauron_unit_test)
