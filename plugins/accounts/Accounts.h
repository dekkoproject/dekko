#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#include <QObject>
#include <qmailaccount.h>
#include <qmailaccountkey.h>
#include <QmlObjectListModel.h>

#include "Account.h"

/** @short Accounts class is the main interface for accessing all account related activities.
 *
 * It includes a child "model" property which provides a QList style model that can be used to access specific account actions
 */
class Accounts : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Filters filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QObject *model READ model NOTIFY modelChanged)
    Q_ENUMS(Filters)
public:
    explicit Accounts(QObject *parent = 0);

    enum Filters {
        Enabled,
        CanReceive,
        CanSend,
        Synchronized,
        HasPersitentConnection
    };

    static quint64 maskForFilter(Accounts::Filters filter);
    Filters filter() const;
    QObject *model() { return m_model; }

signals:
    void filterChanged(Filters filter);
    void modelChanged();

public slots:
    void setFilter(Filters filter);

private slots:
    void accountsAdded(const QMailAccountIdList &ids);
    void accountsUpdated(const QMailAccountIdList &ids);
    void accountsRemoved(const QMailAccountIdList &ids);

private:
    Filters m_filter;
    QQmlObjectListModel<Account> *m_model;
    QMailAccountIdList m_idList;
    void reset();
    void init();
};

#endif // ACCOUNTS_H
