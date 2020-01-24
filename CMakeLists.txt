cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

link_directories(lib)
include_directories(include)

project(simple-chat-common)
add_library(${CMAKE_PROJECT_NAME} STATIC common.c c-vector/Vector.c)

project(simple-chat-server)
add_executable(${CMAKE_PROJECT_NAME} server.c)
target_link_libraries(${CMAKE_PROJECT_NAME} simple-chat-common pthread)

project(simple-chat-server-cpp)
add_executable(${CMAKE_PROJECT_NAME} server.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} simple-chat-common pthread sockpp)

project(simple-chat-client)
add_executable(${CMAKE_PROJECT_NAME} client.c)
target_link_libraries(${CMAKE_PROJECT_NAME} simple-chat-common)
