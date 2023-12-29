cmake_minimum_required(VERSION 3.16)

project(TestCmake VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS Quick)



qt_add_executable(appTestCmake
    main.cpp
)

qt_add_qml_module(appTestCmake
    URI TestCmake
    VERSION 1.0
    NO_PLUGIN
    QML_FILES Main.qml
)


set_target_properties(appTestCmake PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

add_subdirectory(second)

target_link_libraries(appTestCmake
    PRIVATE Qt6::Quick
    secondplugin #qt_add_qml_module里面的模块
    second  # 正常模块为add_library的模块。不加则找不到里面的Cpp文件
)

include(GNUInstallDirs)
install(TARGETS appTestCmake
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
