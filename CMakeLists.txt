cmake_minimum_required(VERSION 2.6)
project(rc)

add_executable(server server.c)
add_executable(client client.c)

add_subdirectory(orquestrador)

#TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${USED_LIBS})

