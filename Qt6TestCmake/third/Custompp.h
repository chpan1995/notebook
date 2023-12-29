#ifndef CUSTOMPP_H
#define CUSTOMPP_H

#include <QObject>
#include <second/Rejest.h>

class Custompp : public QObject
{
    Q_OBJECT
public:
    explicit Custompp(QObject *parent = nullptr);

signals:
private:
    Rejest re;
};

#endif // CUSTOMPP_H
