#ifndef ELEMENTPRO_H
#define ELEMENTPRO_H

#include <QObject>
#include <QQmlEngine>

class ElementPro : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit ElementPro(QObject *parent = nullptr);

signals:
};

#endif // ELEMENTPRO_H
