cmake_minimum_required(VERSION 3.13.2)
project(ipc_chat)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wextra ")

add_executable(${PROJECT_NAME}
        main.cpp
        Server.cpp
        Server.hpp
        Client.cpp
        Client.hpp
        )