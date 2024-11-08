cmake_minimum_required(VERSION 3.16)

project(third VERSION 0.1 LANGUAGES CXX)

# set(CMAKE_AUTOMOC ON)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(Qt6 6.2 REQUIRED COMPONENTS Quick)
qt6_policy(SET QTP0001 NEW)

qt_add_library(${PROJECT_NAME} STATIC
    # Custompp.h Custompp.cpp
    # Rejest.h Rejest.cpp
)

qt_add_qml_module(${PROJECT_NAME}
    URI third #为了匹配目标路径的结构，您必须将目录命名与模块 URI 的段完全一样
    VERSION 1.0
    PLUGIN_TARGET thirdplugin
    QML_FILES ThridTst.qml
    SOURCES ElementPro.h ElementPro.cpp
)

set_target_properties(appTestCmake PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${CMAKE_SOURCE_DIR}
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

