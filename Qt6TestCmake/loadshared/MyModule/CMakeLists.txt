cmake_minimum_required(VERSION 3.16)

project(MyModule VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

# 必须加这个而且为静态库
qt_add_library(MyModule STATIC)

qt_add_qml_module(MyModule
    URI MyNamespace.MyModule
    VERSION 1.0
    RESOURCE_PREFIX "/"
    SOURCES
        MyModule.h
        mymodule.cpp
    QML_FILES
        MyComponent.qml

)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${CMAKE_SOURCE_DIR}
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)
install(TARGETS MyModule DESTINATION "${CMAKE_INSTALL_PREFIX}/MyModule")
