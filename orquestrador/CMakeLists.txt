cmake_minimum_required(VERSION 2.6)
project(rc)

add_subdirectory(arquivado)


add_executable(listenerCliente listenerCliente.c)
add_executable(listenerServidor listenerServidor.c)
add_executable(senderOrquestrador senderOrquestrador.c)


#TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${USED_LIBS})

