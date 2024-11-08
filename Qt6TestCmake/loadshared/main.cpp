#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "MyModule/MyModule.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // 设置 QML 模块的搜索路径
    engine.addImportPath(QStringLiteral("qrc:/"));

    // 加载主 QML 文件
    engine.load(QUrl(QStringLiteral("qrc:/MainApp/Main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;
    MyModule my;

    return app.exec();
}
