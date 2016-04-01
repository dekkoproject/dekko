#include "Client.h"
#include <QDebug>
#include <QPointer>

static QPointer<Client> s_client;
Client *Client::instance()
{
    if (s_client.isNull()) {
        s_client = new Client();
    }
    return s_client;
}

QObject *Client::factory(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    return Client::instance();
}

Client::Client(QObject *parent) : QObject(parent),
    m_service(0)
{
    m_service = new ClientService(this);
    emit serviceChanged();
}

void Client::deleteMessage(const int &msgId)
{
    qDebug() << "Msg id to delete is: " << msgId;
    deleteMessages(QMailMessageIdList() << QMailMessageId(msgId));
}

void Client::deleteMessages(const QMailMessageIdList &idList)
{
    m_service->deleteMessages(idList);
}

void Client::restoreMessage(const int &msgId)
{
    m_service->restoreMessage(QMailMessageId(msgId));
}

void Client::markMessageImportant(const int &msgId, const bool important)
{
    markMessagesImportant(QMailMessageIdList() << QMailMessageId(msgId), important);
}

void Client::markMessageRead(const int &msgId, const bool read)
{
    markMessagesRead(QMailMessageIdList() << QMailMessageId(msgId), read);
}

void Client::markMessageTodo(const int &msgId, const bool todo)
{
    markMessagesTodo(QMailMessageIdList() << QMailMessageId(msgId), todo);
}

void Client::markMessagesImportant(const QMailMessageIdList &idList, const bool important)
{
    m_service->markMessagesImportant(idList, important);
}

void Client::markMessagesRead(const QMailMessageIdList &idList, const bool read)
{
    m_service->markMessagesRead(idList, read);
}

void Client::markMessagesTodo(const QMailMessageIdList &idList, const bool todo)
{
    m_service->markMessagesTodo(idList, todo);
}

