add_definitions(-DBOOST_TEST_DYN_LINK)

include_directories(${CMAKE_SOURCE_DIR}/src)

#SAURON INTEGRATION TESTS EXECUTABLE
set(SOURCE_FILES_SAURON_INTEGRATION_TEST
        IntegrationTestMain.cpp
        TestClient.cpp
        )

add_executable(sauron_integration_test
        ${SOURCE_FILES_SAURON_INTEGRATION_TEST})

target_link_libraries(sauron_integration_test
        ${CMAKE_THREAD_LIBS_INIT}
        sauron_lib
        sauron_tests_common
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
        yaml-cpp
        )

add_test(sauron_integration_test sauron_integration_test)
