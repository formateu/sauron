add_definitions(-DBOOST_TEST_DYN_LINK)

include_directories(${CMAKE_SOURCE_DIR}/src)

#SAURON UNIT TESTS EXECUTABLE
set(SOURCE_FILES_SAURON_UNIT_TEST
        UnitTestMain.cpp
        TestClient.cpp
        TestMessageBuffer.cpp
        )

add_executable(sauron_unit_test
        ${SOURCE_FILES_SAURON_UNIT_TEST})

target_link_libraries(sauron_unit_test
        ${CMAKE_THREAD_LIBS_INIT}
        sauron_lib
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
        yaml-cpp
        )

add_test(sauron_unit_test sauron_unit_test)