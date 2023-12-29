#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QtQml/qqmlextensionplugin.h>
#include <Rejest.h>
#include <ElementPro.h>

// 导入其它QML的插件(com_SecondPlugin)为URI
// 这里是second里面的add_qml_modules
Q_IMPORT_QML_PLUGIN(com_SecondPlugin)

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/TestCmake/Main.qml"_qs);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.addImportPath(":/"); // ':/'插件的前缀不然使用不了second里面的qml
    Rejest re;
    ElementPro ele;

    engine.load(url);

    return app.exec();
}
