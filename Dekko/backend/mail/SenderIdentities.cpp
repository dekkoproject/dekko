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
#include "SenderIdentities.h"
#include <qmailaccount.h>

SenderIdentities::SenderIdentities(QObject *parent) : QObject(parent), m_selectedIndex(-1),
    m_accountsModel(Q_NULLPTR), m_accountIdentities(Q_NULLPTR), m_model(Q_NULLPTR)
{
    m_model = new QQmlObjectListModel<IdentityWrapper>(this);
}

QObject *SenderIdentities::model() const { return m_model; }

int SenderIdentities::selectedIndex() const
{
    return m_selectedIndex;
}

QObject *SenderIdentities::selectedAccount() const
{
    if (m_selectedIndex < 0 || m_selectedIndex > m_model->count()) {
        return new QObject();
    }
    return m_model->at(m_selectedIndex);
}

QObject *SenderIdentities::accountsModel() const
{
    return m_accountsModel;
}

bool SenderIdentities::isEmpty()
{
    return m_model->isEmpty();
}

bool SenderIdentities::hasAddress(const QString &address)
{
    foreach(IdentityWrapper *w, m_model->toList()) {
        if (w->get_identity()->get_email() == address) {
            return true;
        }
    }
    return false;
}

Identities *SenderIdentities::identities() const
{
    return m_accountIdentities;
}

int SenderIdentities::determinePreferredIdentity(const QMailMessage &message)
{
    qDebug() << __func__;
    int preferred = -1;
    if (m_model->isEmpty()) {
        return preferred;
    }
    // find the account were after
    foreach(Account *account, m_accountsModel->toList()) {
        if (account->accountId() != message.parentAccountId()) {
            continue;
        }
        // first see if we can match an identity to the message recipients
        const int pi = m_accountIdentities->determineBestIdentityFromMessage(message);
        if (pi > 0) {
            preferred = findIndexOf(pi);
            break;
        }

        if (preferred == -1) {
        // else see if an identity for this account has the default flag
            QVariantMap d = m_accountIdentities->get(m_accountIdentities->defaultIdentity());
            if (d["parentId"].toUInt() == message.parentAccountId().toULongLong()) {
                preferred = findIndexOf(d["id"].toInt());
                break;
            }
        }

        if (preferred == -1) {
            auto ids = m_accountIdentities->getAllForAccount(message.parentAccountId().toULongLong());
            int id = ids.first().value("id").toInt();
            preferred = findIndexOf(id);
        }
    }
    return preferred;
}

void SenderIdentities::setSelectedIndex(int selectedIndex)
{
    if (m_selectedIndex == selectedIndex)
        return;

    m_selectedIndex = selectedIndex;
    qDebug() << "SelectedIndex: " << m_selectedIndex;
    emit selectedIndexChanged();
}

void SenderIdentities::setAccountsModel(QObject *accountsModel)
{
    if (accountsModel == Q_NULLPTR) {
        return;
    }
    if (m_accountsModel) {
        disconnect(m_accountsModel, 0, this, 0);
    }
    QQmlObjectListModel<Account> *model = static_cast<QQmlObjectListModel<Account> *>(accountsModel);
    if (m_accountsModel == model)
        return;

    m_accountsModel = model;
    connect(m_accountsModel, &QQmlObjectListModel<Account>::countChanged, this, &SenderIdentities::accountsChanged);
    emit modelsChanged();
    reset();
}

void SenderIdentities::setSelectedIndexFromAccountId(quint64 accountId)
{
    qDebug() << __func__;
    if (m_model->isEmpty()) {
        setSelectedIndex(-1);
        return;
    }
    // find the account were after
    foreach(Account *account, m_accountsModel->toList()) {
        if (account->accountId() == QMailAccountId(accountId)) {
            setSelectedIndex(m_accountsModel->indexOf(account));
            return;
        }
    }
}

void SenderIdentities::reset()
{
    m_model->clear();
    if (!m_accountIdentities || !m_accountsModel) {
        return;
    }
    foreach(Account *account, m_accountsModel->toList()) {
        IdentityList il = m_accountIdentities->getAllForAccount(account->accountId().toULongLong());
        if (il.isEmpty()) {
            // Hmm there are no identities but the accounts is still enabled.
            // we need to fall back to the outgoing data in the mailstore
            m_model->append(createIdentityFromAccount(account));
        } else {
            for (const QVariantMap &i : il) {
                m_model->append(createIdentity(account, i));
            }
        }
    }
    //TODO set default index/identity
    setSelectedIndex(findIndexOf(m_accountIdentities->defaultIdentity()));
}

void SenderIdentities::setIdentities(Identities *accountIdentities)
{
    if (m_accountIdentities == accountIdentities)
        return;

    m_accountIdentities = accountIdentities;
    connect(m_accountIdentities, &Identities::identitiesChanged, this , &SenderIdentities::reset);
    emit modelsChanged();
    reset();
}

void SenderIdentities::accountsChanged()
{
    reset();
}

int SenderIdentities::findIndexOf(const quint64 &id)
{
    foreach(IdentityWrapper *identity, m_model->toList()) {
        if (identity->get_identity()->get_accountId() == id) {
            return m_model->indexOf(identity);
        }
    }
    return -1;
}

IdentityWrapper* SenderIdentities::createIdentityFromAccount(Account *account)
{
    IdentityWrapper *wrapper = new IdentityWrapper(Q_NULLPTR);
    wrapper->set_account(account);
    Identity *identity = new Identity(wrapper);
    identity->set_accountId(100 + m_model->count());
    identity->set_parentId(account->accountId().toULongLong());
    identity->set_name(account->qMailAccount()->name());
    identity->set_email(account->qMailAccount()->fromAddress().toString());
    identity->set_signature(account->qMailAccount()->signature());
    wrapper->set_identity(identity);
    return wrapper;
}

IdentityWrapper* SenderIdentities::createIdentity(Account *account, const QVariantMap &identity)
{
    IdentityWrapper *wrapper = new IdentityWrapper(Q_NULLPTR);
    wrapper->set_account(account);
    wrapper->set_identity(Identity::fromMap(wrapper, identity));
    return wrapper;
}


