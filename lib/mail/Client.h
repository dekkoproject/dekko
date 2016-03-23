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

    // QML API
    Q_INVOKABLE void deleteMessage(const int &msgId);
    Q_INVOKABLE void restoreMessage(const int &msgId);
    Q_INVOKABLE void markMessageImportant(const int &msgId, const bool important);
    Q_INVOKABLE void markMessageRead(const int &msgId, const bool read);

    // C++ Extras API
    void deleteMessages(const QMailMessageIdList &idList);
    void markMessagesImportant(const QMailMessageIdList &idList, const bool important);
    void markMessagesRead(const QMailMessageIdList &idList, const bool read);

signals:
    void serviceChanged();

public slots:

private:
    ClientService *m_service;
};

#endif // CLIENT_H
