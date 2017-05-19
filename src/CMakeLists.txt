set(SOURCE_FILES_SAURON
        Server.cpp
        HalfRing.cpp
        main_server.cpp
        Connector.cpp
        MessageBuffer.cpp
        Config.cpp
        )

set(SOURCE_FILES_CLIENT
        main_client.cpp
        Timer.cpp
        Client.cpp
        Connector.cpp
        MessageBuffer.cpp
        ClientState.cpp
        )

set(SOURCE_FILES_SERVER
        Server.cpp
        HalfRing.cpp
        Connector.cpp
        MessageBuffer.cpp
        ServerState.cpp
        )

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_library(sauron_lib
        ${SOURCE_FILES_SAURON}
        ${SOURCE_FILES_CLIENT}
        ${SOURCE_FILES_SERVER}
        )

#SAURON EXECUTABLE
add_executable(sauron
        ${SOURCE_FILES_SAURON})

target_link_libraries(sauron
        ${CMAKE_THREAD_LIBS_INIT}
	${Boost_LIBRARIES}
        yaml-cpp
        )

#CLIENT EXECUTABLE
add_executable(sauron_client
        ${SOURCE_FILES_CLIENT})

target_link_libraries(sauron_client
	LINK_PUBLIC
        ${CMAKE_THREAD_LIBS_INIT}
	${Boost_SYSTEM_LIBRARY}
	${Boost_LIBRARIES}
        )
