add_definitions(-DBOOST_TEST_DYN_LINK)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/test/common)

#SAURON UNIT TESTS EXECUTABLE
set(SOURCE_FILES_SAURON_UNIT_TEST
        UnitTestMain.cpp
        TestMessageBuffer.cpp
        TestClient.cpp
        TestServer.cpp
        TestInternetConnector.cpp
        TestTimer.cpp
        TestConfig.cpp
        )

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_executable(sauron_unit_test
        ${SOURCE_FILES_SAURON_UNIT_TEST})

target_link_libraries(sauron_unit_test
	LINK_PUBLIC
        ${CMAKE_THREAD_LIBS_INIT}
        sauron_lib
        sauron_tests_common
	${Boost_SYSTEM_LIBRARY}
	${Boost_LIBRARIES}
        yaml-cpp
        )

add_test(sauron_unit_test sauron_unit_test)
