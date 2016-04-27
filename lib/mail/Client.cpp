#include "Client.h"
#include <QDebug>
#include <QPointer>
#include <qmailstore.h>

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
    connect(m_service, &ClientService::messagePartFetched, this, &Client::messagePartNowAvailable);
    connect(m_service, &ClientService::messagePartFetchFailed, this, &Client::messagePartFetchFailed);
    connect(m_service, &ClientService::messagesFetched, this, &Client::messagesNowAvailable);
    connect(m_service, &ClientService::messageFetchFailed, this, &Client::messageFetchFailed);
}

bool Client::hasConfiguredAccounts()
{
    return QMailStore::instance()->countAccounts() > 0;
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

void Client::markMessageDone(const int &msgId, const bool done)
{
    markMessagesDone(QMailMessageIdList() << QMailMessageId(msgId), done);
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

void Client::markMessagesDone(const QMailMessageIdList &idList, const bool done)
{
    m_service->markMessagesDone(idList, done);
}

void Client::downloadMessagePart(const QMailMessagePart *msgPart)
{
    qDebug() << "[Client]" << "Downloading message part" << msgPart->location().toString(true);
    m_service->downloadMessagePart(msgPart);
}


void Client::downloadMessage(const QMailMessageId &msgId)
{
    downloadMessages(QMailMessageIdList() << msgId);
}

void Client::downloadMessages(const QMailMessageIdList &idList)
{

}
