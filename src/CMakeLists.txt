set(SOURCE_FILES_SAURON
        main.cpp
        Connector.cpp
        MessageBuffer.cpp
        Config.cpp
        )

set(SOURCE_FILES_CLIENT
        main_client.cpp
        Client.cpp
        Connector.cpp
        MessageBuffer.cpp
        ClientState.cpp
        )

add_library(sauron_lib
        ${SOURCE_FILES_SAURON}
        ${SOURCE_FILES_CLIENT})

#SAURON EXECUTABLE
add_executable(sauron
        ${SOURCE_FILES_SAURON})

target_link_libraries(sauron
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
        yaml-cpp
        )

#CLIENT EXECUTABLE
add_executable(sauron_client
        ${SOURCE_FILES_CLIENT})

target_link_libraries(sauron_client
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
        )