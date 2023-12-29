#include "Custompp.h"
#include <QDebug>
Custompp::Custompp(QObject *parent)
    : QObject{parent}
{
    qDebug() << re.metaObject()->className();
}
