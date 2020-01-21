project(simple-chat-common)
add_library(${CMAKE_PROJECT_NAME} STATIC common.c c-vector/Vector.c)

project(simple-chat-server)
add_executable(${CMAKE_PROJECT_NAME} server.c)
target_link_libraries(${CMAKE_PROJECT_NAME} simple-chat-common)

project(simple-chat-client)
add_executable(${CMAKE_PROJECT_NAME} client.c)
target_link_libraries(${CMAKE_PROJECT_NAME} simple-chat-common)
