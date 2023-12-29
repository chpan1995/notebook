#ifndef REJEST_H
#define REJEST_H

#include <QObject>
#include <QQmlEngine>

class Rejest : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit Rejest(QObject *parent = nullptr);

signals:
    void sigreg();
};

#endif // REJEST_H
