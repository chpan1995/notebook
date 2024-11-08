#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QtQml/qqmlextensionplugin.h>
#include <Rejest.h>
#include <QTimer>
#include <ElementPro.h>
// #include <third/Custompp.h>

// 导入其它QML的插件(com_SecondPlugin)为URI
// 这里是second里面的add_qml_modules
// Q_IMPORT_QML_PLUGIN(secondPlugin)
// Q_IMPORT_QML_PLUGIN(thirdPlugin)

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url("qrc:/TestCmake/Main.qml");
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
    Rejest re;    // 需在cmake中 添加second 与C++是使用规则相同
    ElementPro ele; // 需在cmake中secondplugin 他是add_qml_modules的
    // Custompp cuspp;

    qmlRegisterType<Rejest>("my.rej",1,0,"Rejest");
    engine.rootContext()->setContextProperty("re",&re);

    engine.load(url);

    QTimer::singleShot(2000,[&]{re.sigreg();});

    return app.exec();
}
