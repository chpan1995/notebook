# 信号和槽 注意事项
1. 信号槽connect的时候扩展有5种，在跨线程使用的时候选择不同的参数会有不同的效果，使用不当会造成线程同步甚至崩溃的问题。5种参数的意义不再累述，可以参考 信号槽的多线程安全性讨论
2. 第一个坑  跨线程时connect使用Direct Connection （直接连接）。跨线程触发槽的时候，因为信号和槽不在一个线程内需要考虑同步问题（参考 信号槽的多线程安全性讨论），比较简单的方式是槽函数加全局锁。除此之外还有一个更简单的方式，就是connect的使用Direct Connection （直接连接），这种连接方式将会使槽函数在信号发出的线程中直接执行，这种情况业务处理实际就变成了单线程，并发问题自然没有了。但是这种情况有一个致命问题，比如A，B分属两个线程，A使用直连方式调用B的槽函数，其实是在A线程内创建了一个B的临时对象，然后调用B的槽函数。B的临时对象是使用默认构造函数创建的，如果B没有默认构造函数，这种情况运行程序将会直接崩溃。qt官方文档上也有说明这一使用方式的不安全性，只是没有说明具体的原因。建议跨线程时不要使用Direct Connection去解决同步问题。
3. 第二个坑 跨线程时connect使用 Queued Connection（队列连接）信号触发太频繁而槽函数处理时间又太长,队列连接的时候，信号传递的参数存放在槽函数所在线程的消息队列里，这个消息队列基于事件机制向对应线程分发任务，这个过程是线程阻塞的。如果信号频繁触发，而槽函数处理的时间又太长，长过信号触发的间隔时间，就会造成消息队列不停增长，内存不断增加直到崩溃。这种情况没什么太好的解决方式，只能是在程序设计的时候尽量避免。
4. 信号槽在提供便利性的同时不仅牺牲了效率，而且造成了使用的复杂性，qt中类似的坑有很多。个人感觉qt的优势还是在于界面，非界面开发的时候综合开发体验还是比c++11和boost差的太多了，这又是另一个话题了。
# GDB
1. 在终端中启动 gdb，指定主程序和库文件：
```
gdb -ex "file main" -ex "file /path/to/lib.so"
rm -rf ~/.config/navicat
rm -rf ~/.config/dconf/user
```
# Qt的槽里面执行exec的话，主事件循环正常运行，但是绑定到槽的exec执行完才能继续处理这个信号。(就这个信号会等待槽执行完，才能继续处理这个信号关联的其他槽)其它信号正常处理

# qmlDir导入问题
1. cmake 配置 需在RESOURCES加入导入的文件
```
qt_add_qml_module(appuntitled4
    URI untitled4
    NO_RESOURCE_TARGET_PATH
    VERSION 1.0
    QML_FILES
    Main.qml
    # imports/Untitled3/HAle.qml
    # imports/Untitled3/Best.qml
    # imports/Untitled3/qmldir
    # RESOURCE_PREFIX /untitled3
    RESOURCES
    imports/Untitled3/HAle.qml
    imports/Untitled3/Best.qml
    imports/Untitled3/qmldir
)
```
2. 代码配置
``` 
// 通过 qt_add_qml_module 引入别的qmlDir
engine.addImportPath("qrc:/imports");
const QUrl url(u"qrc:/Main.qml"_qs);
// 不通过 qt_add_qml_module 引入 qmlDir
// engine.addImportPath("/home/chpan/code/demoCode/untitled4/imports");
// const QUrl url(u"qrc:/Hale.qml"_qs);

// 通过ddImportPath("qrc:/qrc") 在qt_add_qml_module的Mainq.qml调用主的qrc里面的qml文件
// 总结： 都是通过addImportPath 配置引用别的模块的qml
engine.addImportPath("qrc:/qrc");
```

# Qt6 添加子CMakeLists.txt 的qt_add_qml_module
- 主CMake的target_link_libraries 要加secondplugin second </br>
  这样生成了两个库  libsecondplugin.a libsecond.a
- 导入secondplugin Q_IMPORT_QML_PLUGIN(com_SecondPlugin)

- addImportPath(":/"); // ':/'插件的前缀不然使用不了second里面的qml
