#ifndef ACCOUNTSERVICECLIENT_H
#define ACCOUNTSERVICECLIENT_H

#include <QObject>
#include <QtDBus>
#include <QDBusPendingReply>
#include <qmailaccountkey.h>
#include <qmailaccountsortkey.h>
#include "AccountServiceInterface.h"

typedef QDBusPendingReply<QList<quint64> > IdReply;

class AccountServiceClient : public QObject
{
    Q_OBJECT
public:
    explicit AccountServiceClient(QObject *parent = 0);

    void removeAccount(const QMailAccountId &id);

    IdReply queryAccounts(const QMailAccountKey &accountKey = QMailAccountKey(), const QMailAccountSortKey &sortKey = QMailAccountSortKey(), const int &limit = 0);

signals:
    void accountRemoved(QMailAccountId &id);

private slots:
    void handleRemoved(const quint64 &id);

private:
    AccountServiceInterface *m_service;
};

#endif // ACCOUNTSERVICECLIENT_H
