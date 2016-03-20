#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>
#include "ClientService.h"

class Client : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *service READ service NOTIFY serviceChanged)
public:
    static Client *instance();
    static QObject *factory(QQmlEngine *engine, QJSEngine *scriptEngine);
    explicit Client(QObject *parent = 0);

    QObject *service() const { return m_service; }

    Q_INVOKABLE void deleteMessage(const int &msgId);
    void deleteMessages(const QMailMessageIdList &idList);
    Q_INVOKABLE void restoreMessage(const int &msgId);

signals:
    void serviceChanged();

public slots:

private:
    ClientService *m_service;
};

#endif // CLIENT_H
