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

