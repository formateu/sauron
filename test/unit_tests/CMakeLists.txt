#SAURON UNIT TESTS EXECUTABLE
set(SOURCE_FILES_SAURON_UNIT_TEST
        UnitTestMain.cpp
        TestClient.cpp
        TestMessageBuffer.cpp
        )

add_executable(sauron_unit_test
        #${HEADER_FILES_SAURON}
        ${SOURCE_FILES_SAURON_UNIT_TEST})

target_link_libraries(sauron_unit_test
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
        yaml-cpp
        )