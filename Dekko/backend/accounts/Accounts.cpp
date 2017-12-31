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
#include "Accounts.h"
#include <QDebug>
#include "qmailstore.h"

Q_LOGGING_CATEGORY(D_ACCOUNTS_LIST, "dekko.accounts.list")

Accounts::Accounts(QObject *parent) : QObject(parent),
    m_filter(Enabled), m_model(0)
{
    m_model = new QQmlObjectListModel<Account>(this);
    emit modelChanged();
    connect(QMailStore::instance(),
            SIGNAL(accountsAdded(QMailAccountIdList)),
            this,
            SLOT(accountsAdded(QMailAccountIdList)));
    connect(QMailStore::instance(),
            SIGNAL(accountsRemoved(QMailAccountIdList)),
            this,
            SLOT(accountsRemoved(QMailAccountIdList)));
    connect(QMailStore::instance(),
            SIGNAL(accountsUpdated(QMailAccountIdList)),
            this,
            SLOT(accountsUpdated(QMailAccountIdList)));
    reset();
}

quint64 Accounts::maskForFilter(Accounts::Filters filter)
{
    switch(filter) {
    case Enabled:
        return QMailAccount::Enabled;
    case CanReceive:
        return QMailAccount::MessageSource;
    case CanSend:
        return QMailAccount::MessageSink;
    case Synchronized:
        return QMailAccount::Synchronized;
    case HasPersitentConnection:
        return QMailAccount::HasPersistentConnection;
    }
    Q_UNREACHABLE();
}

Accounts::Filters Accounts::filter() const
{
    return m_filter;
}

bool Accounts::deleteAccount(const quint64 &id)
{
    return QMailStore::instance()->removeAccount(QMailAccountId(id));
}

bool Accounts::hasAccount(const quint64 &id)
{
    foreach(const Account *a, m_model->toList()) {
        if (a->accountId().toULongLong() == id) {
            return true;
        }
    }
    return false;
}

Account *Accounts::get(const quint64 &id)
{
    foreach(Account *a, m_model->toList()) {
        if (a->accountId().toULongLong() == id) {
            return a;
        }
    }
    return new Account();
}

void Accounts::setFilter(Accounts::Filters filter)
{
    if (m_filter == filter)
        return;

    m_filter = filter;
    reset();
    emit filterChanged(filter);
}

void Accounts::accountsAdded(const QMailAccountIdList &ids)
{
    qCDebug(D_ACCOUNTS_LIST) << "Adding accounts " << ids << "to list";
    QMailAccountKey passKey;
    if (filter() != Enabled)
        // We need to ensure we only show enabled accounts
        // This means we can do filters like CanReceive | Enabled
        passKey = QMailAccountKey::status(maskForFilter(filter()) | QMailAccount::Enabled) & QMailAccountKey::id(ids);
    else
        passKey = QMailAccountKey::status(maskForFilter(filter())) & QMailAccountKey::id(ids);

    QMailAccountIdList results = QMailStore::instance()->queryAccounts(passKey);
    if(results.isEmpty())
        return;

    // TODO: once sortkey is implemented update the model with the sorted result
    // for now we just append the new account
    Q_FOREACH(const QMailAccountId &id, ids) {
        if (!m_idList.contains(id)) {
            Account *account = new Account();
            account->setId(id.toULongLong());
            m_model->append(account);
            m_idList.append(id);
        } else {
            qCDebug(D_ACCOUNTS_LIST) << __func__ << "Account with same id already in model";
        }
    }
}

void Accounts::accountsUpdated(const QMailAccountIdList &ids)
{
    QMailAccountKey idKey(QMailAccountKey::id(ids));
    QMailAccountIdList validIds = QMailStore::instance()->queryAccounts(idKey & QMailAccountKey::status(maskForFilter(filter())));
    // We maybe here because a an account has been enabled/disabled or some other filter we use
    // in this model and we now need to remove or add an account to the model

    // remove any that shouldn't be in the model
    Q_FOREACH(const QMailAccountId &id, ids) {

        if (!validIds.contains(id)) {
            int index = m_idList.indexOf(id);
            if (index == -1) {
                continue;
            }
            m_model->remove(index);
            m_idList.removeAt(index);
        }
    }

    // go over the valid ids and either insert any missing or update any existing
    Q_FOREACH(const QMailAccountId &id, validIds) {
        int index = m_idList.indexOf(id);
        if (index == -1) {
            // Not here so create.
            Account *account = new Account();
            account->setId(id.toULongLong());
            m_model->append(account);
            m_idList.append(id);
        } else {
            // just notify somethings changed
            m_model->at(index)->emitAccountChanged();
        }
    }
}

void Accounts::accountsRemoved(const QMailAccountIdList &ids)
{
    qCDebug(D_ACCOUNTS_LIST) << "Removing accounts" << ids << "from list";
    Q_FOREACH(const QMailAccountId &id, ids) {
        int index = m_idList.indexOf(id);
        if (index == -1) {
            // This id#s not in this model
            continue;
        }
        // Remove both from model & idlist;
        m_model->remove(index);
        m_idList.removeAt(index);
    }
}

void Accounts::reset()
{
    qCDebug(D_ACCOUNTS_LIST) << "Resetting accounts list";
    m_model->clear();
    m_idList.clear();
    init();
}

void Accounts::init()
{
    qCDebug(D_ACCOUNTS_LIST) << "Initialising accounts list";
    auto idList = QMailStore::instance()->queryAccounts(QMailAccountKey::status(maskForFilter(filter())) ,QMailAccountSortKey::name());
    Q_FOREACH(const QMailAccountId &id, idList) {
        int index = m_idList.indexOf(id);
        if (index == -1) {
            // Not here so create.
            Account *account = new Account(this);
            account->setId(id.toULongLong());
            m_model->append(account);
            m_idList.append(id);
        }
    }
}

