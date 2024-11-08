import QtQuick
import MyNamespace.MyModule 1.0  // 动态加载子模块

Window {
    visible: true
    width: 640
    height: 480
    title: "QML Module Example"

    MyComponent { }  // 使用子模块中的组件
}
