cmake_minimum_required(VERSION 2.6)
project(rc)

set(HEADERS
    mensagem.h
)

add_executable(orquestradorUDP orquestradorUDP.c ${HEADERS})
add_executable(servidor servidor.c ${HEADERS})
add_executable(clienteTCP clienteTCP.c ${HEADERS})

add_subdirectory(orquestrador)

#TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${USED_LIBS})

