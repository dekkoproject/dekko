#include "AccountServiceClient.h"
#include <QDBusConnection>
#include "serviceutils.h"

#define ACCOUNTS_SERVICE "org.dekkoproject.Service"
#define ACCOUNTS_PATH "/accounts"

AccountServiceClient::AccountServiceClient(QObject *parent) : QObject(parent),
    m_service(0)
{
    m_service = new AccountServiceInterface(ACCOUNTS_SERVICE, ACCOUNTS_PATH, QDBusConnection::sessionBus());
    connect(m_service, &AccountServiceInterface::accountRemoved, this, &AccountServiceClient::handleRemoved);
}

void AccountServiceClient::removeAccount(const QMailAccountId &id)
{
    m_service->removeAccount(id.toULongLong());
}

IdReply AccountServiceClient::queryAccounts(const QMailAccountKey &accountKey, const QMailAccountSortKey &sortKey, const int &limit)
{
    return m_service->queryAccounts(account_key_bytes(accountKey), account_sort_key_bytes(sortKey), limit);
}

void AccountServiceClient::handleRemoved(const quint64 &id)
{
    QMailAccountId aId(id);
    emit accountRemoved(aId);
}
