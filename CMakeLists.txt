cmake_minimum_required(VERSION 2.6)
project(rc)

add_executable(orquestradorUDP orquestradorUDP.c)
add_executable(servidor servidor.c)
add_executable(clienteTCP clienteTCP.c)

add_subdirectory(orquestrador)

#TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${USED_LIBS})

