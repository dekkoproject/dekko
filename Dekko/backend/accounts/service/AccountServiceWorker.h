#ifndef ACCOUNTSERVICEWORKER_H
#define ACCOUNTSERVICEWORKER_H

#include <QObject>
#include <QtDBus>

class AccountServiceWorker : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.dekkoproject.AccountService")
public:
    explicit AccountServiceWorker(QObject *parent = 0);

public slots:
    void removeAccount(const quint64 &id);

    QList<quint64> queryAccounts(const QByteArray &accountKey = QByteArray(), const QByteArray &sortKey = QByteArray(), const int &limit = 0);

signals:
    void accountRemoved(const quint64 &id);
};

#endif // ACCOUNTSERVICEWORKER_H
