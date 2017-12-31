/* Copyright (C) 2016 - 2017 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu devices

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#include <QDebug>
#include <QLoggingCategory>
#include <QObject>
#include <qmailaccount.h>
#include <qmailaccountkey.h>
#include <QmlObjectListModel.h>

#include "Account.h"

Q_DECLARE_LOGGING_CATEGORY(D_ACCOUNTS_LIST)

/** @short Accounts class is the main interface for accessing all account related activities.
 *
 * It includes a child "model" property which provides a QList style model that can be used to access specific account actions
 *
 * \ingroup group_accounts
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
    QQmlObjectListModel<Account> *internalModel() { return m_model; }

    Q_INVOKABLE bool deleteAccount(const quint64 &id);
    Q_INVOKABLE bool hasAccount(const quint64 &id);
    Q_INVOKABLE Account *get(const quint64 &id);

signals:
    void filterChanged(Filters filter);
    void modelChanged();

public slots:
    void setFilter(Filters filter);
    void reset();

private slots:
    void accountsAdded(const QMailAccountIdList &ids);
    void accountsUpdated(const QMailAccountIdList &ids);
    void accountsRemoved(const QMailAccountIdList &ids);

private:
    Filters m_filter;
    QQmlObjectListModel<Account> *m_model;
    QMailAccountIdList m_idList;
    void init();
};

#endif // ACCOUNTS_H
