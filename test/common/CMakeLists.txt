include_directories(${CMAKE_SOURCE_DIR}/src)

set(SOURCE_FILES_SAURON_TEST_COMMON
        ClientTestObj.cpp
        )

add_library(sauron_tests_common
        ${SOURCE_FILES_SAURON_TEST_COMMON}
        )
