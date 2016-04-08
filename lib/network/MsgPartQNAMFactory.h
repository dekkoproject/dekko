#ifndef MSGPARTQNAMFACTORY_H
#define MSGPARTQNAMFACTORY_H

#include <QObject>
#include <QQmlNetworkAccessManagerFactory>

class MsgPartQNAMFactory : public QObject, public QQmlNetworkAccessManagerFactory
{
    Q_OBJECT
public:
    virtual QNetworkAccessManager *create(QObject *parent);
};

#endif // MSGPARTQNAMFACTORY_H
