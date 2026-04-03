cmake_minimum_required(VERSION 3.5)

project(server VERSION 0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

add_executable(${PROJECT_NAME} server.h server.cpp main.cpp)

add_executable(http_client_demo http_client.h http_client.cpp http_client_demo.cpp)

add_executable(websocket_client_demo websocket_client.h websocket_client.cpp websocket_client_demo.cpp)

target_link_libraries(${PROJECT_NAME}
    PRIVATE  OpenSSL::SSL OpenSSL::Crypto)
