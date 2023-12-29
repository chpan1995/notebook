cmake_minimum_required(VERSION 3.16)

project(second VERSION 0.1 LANGUAGES CXX)

# set(CMAKE_AUTOMOC ON)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(Qt6 6.2 REQUIRED COMPONENTS Quick)


qt_add_library(${PROJECT_NAME} STATIC
    Rejest.h Rejest.cpp
    )

qt_add_qml_module(${PROJECT_NAME}
    URI com.Second
    VERSION 1.0
    QML_FILES Test.qml
    SOURCES ElementPro.h ElementPro.cpp
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${CMAKE_SOURCE_DIR}
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

