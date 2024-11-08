cmake_minimum_required(VERSION 3.16)
project(MyProject)
set(CMAKE_AUTOMOC ON)
# 查找 Qt 包
find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml Quick)

qt_standard_project_setup(REQUIRES 6.8)

# 添加子目录
add_subdirectory(MyModule)

# 添加主可执行文件
qt_add_executable(MyProject
    main.cpp
)


# 安装 QML 文件和插件
# qt_import_qml_plugins(MyModule)

qt_add_qml_module(MyProject
    URI MainApp
    RESOURCE_PREFIX "/"
    VERSION 1.0
    QML_FILES Main.qml
)

# 链接 Qt 库
target_link_libraries(MyProject PRIVATE Qt6::Core Qt6::Quick Qt6::Qml Qt6::Quick MyModuleplugin)
