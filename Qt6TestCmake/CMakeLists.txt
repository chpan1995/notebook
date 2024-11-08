cmake_minimum_required(VERSION 3.16)
set(QTP0001 NEW)

project(TestCmake VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS Quick)

# qt6_policy(SET QTP0001 NEW)



qt_add_executable(appTestCmake
    main.cpp
)


qt_add_resources(appTestCmake
    "res"
    PREFIX "/images"
    FILES
    1.txt
)

qt_add_qml_module(appTestCmake
    URI TestCmake
    RESOURCE_PREFIX "/"
    # NO_RESOURCE_TARGET_PATH # 不设置qrc前缀
    VERSION 1.0
    # NO_PLUGIN               # 不生成插件，主cmake里面默认不生成so
    QML_FILES Main.qml
)


set_target_properties(appTestCmake PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

add_subdirectory(third)
add_subdirectory(second)

# 主cmake加了secondplugin thirdplugin 后两个可以互相调用，
# 不需要在second，third的cmake中相互引用了
target_link_libraries(appTestCmake
    PRIVATE Qt6::Quick
    secondplugin
     #qt_add_qml_module里面的模块 生成了两个库
    # libsecondplugin.a libsecond.a 这个是add_libary生成
    second  # 正常模块为add_library的模块。不加则找不到里面的Cpp文件
    thirdplugin
    # third
)

include(GNUInstallDirs)
install(TARGETS appTestCmake
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
