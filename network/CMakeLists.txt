cmake_minimum_required(VERSION 3.5)

project(server VERSION 0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置 Boost 版本选择路径
set(BOOST_1_90_PATH "/opt/boost_1_90")
set(BOOST_ROOT ${BOOST_1_90_PATH})
set(Boost_NO_SYSTEM_PATHS ON)

find_package(Boost 1.90 REQUIRED COMPONENTS filesystem)

find_package(OpenSSL REQUIRED)

add_executable(${PROJECT_NAME} server.h server.cpp main.cpp )

add_executable(http_client_demo http_client.h http_client.cpp http_client_demo.cpp)

add_executable(websocket_client_demo websocket_client.h websocket_client.cpp websocket_client_demo.cpp)

target_link_libraries(${PROJECT_NAME}
    PRIVATE  OpenSSL::SSL OpenSSL::Crypto Boost::filesystem)

target_link_libraries(http_client_demo
    PRIVATE  OpenSSL::SSL OpenSSL::Crypto Boost::filesystem)

target_link_libraries(websocket_client_demo
    PRIVATE  OpenSSL::SSL OpenSSL::Crypto  Boost::filesystem)