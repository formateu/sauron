set(HEADER_FILES_SAURON
        Connector.h
        MessageBuffer.h
        Config.h
        Message.h
        )

set(SOURCE_FILES_SAURON
        main.cpp
        Connector.cpp
        MessageBuffer.cpp
        Config.cpp
        )

set(HEADER_FILES_CLIENT
        Message.h
        Client.h
        MessageBuffer.h
        Connector.h
        )

set(SOURCE_FILES_CLIENT
        main_client.cpp
        Client.cpp
        Connector.cpp
        MessageBuffer.cpp
        )

#SAURON EXECUTABLE
add_executable(sauron
        ${HEADER_FILES_SAURON}
        ${SOURCE_FILES_SAURON})

target_link_libraries(sauron
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
        yaml-cpp
        )

#CLIENT EXECUTABLE
add_executable(sauron_client
        ${HEADER_FILES_CLIENT}
        ${SOURCE_FILES_CLIENT})

target_link_libraries(sauron_client
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
        )