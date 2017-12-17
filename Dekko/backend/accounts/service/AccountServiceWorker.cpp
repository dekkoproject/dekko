#include "AccountServiceWorker.h"

#include <qmailaccount.h>
#include <qmailstore.h>
#include "serviceutils.h"

AccountServiceWorker::AccountServiceWorker(QObject *parent) : QObject(parent)
{
    connect(QMailStore::instance(), &QMailStore::accountsRemoved, [=](const QMailAccountIdList &ids) {
        foreach(const QMailAccountId &id, ids) {
            emit accountRemoved(id.toULongLong());
        }
    });
}

void AccountServiceWorker::removeAccount(const quint64 &id)
{
    QMailAccountId aId(id);
    QMailStore::instance()->removeAccount(aId);
}

QList<quint64> AccountServiceWorker::queryAccounts(const QByteArray &accountKey, const QByteArray &sortKey, const int &limit)
{
    QMailAccountIdList accounts = QMailStore::instance()->queryAccounts(
                to_account_key(accountKey),
                to_account_sort_key(sortKey),
                limit);
    return to_dbus_accountlist(accounts);
}
