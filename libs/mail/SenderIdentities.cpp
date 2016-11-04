/* Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

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
    m_accountsModel(Q_NULLPTR)
{
}

int SenderIdentities::selectedIndex() const
{
    return m_selectedIndex;
}

QObject *SenderIdentities::selectedAccount() const
{
    if (m_selectedIndex < 0 || m_selectedIndex > m_accountsModel->count()) {
        return new QObject();
    }
    return m_accountsModel->at(m_selectedIndex);
}

QObject *SenderIdentities::accountsModel() const
{
    return m_accountsModel;
}

bool SenderIdentities::isEmpty()
{
    bool result = true;
    if (m_accountsModel != Q_NULLPTR) {
        result = m_accountsModel->isEmpty();
    }
    return result;
}

bool SenderIdentities::hasAddress(const QString &address)
{
    foreach (Account *account, m_accountsModel->toList()) {
        QMailAddress from = account->qMailAccount()->fromAddress();
        if (from.address() == address) {
            return true;
        }
    }
    return false;
}

void SenderIdentities::setSelectedIndex(int selectedIndex)
{
    if (m_selectedIndex == selectedIndex)
        return;

    m_selectedIndex = selectedIndex;
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
    accountsChanged();
}

void SenderIdentities::setSelectedIndexFromAccountId(quint64 accountId)
{
    qDebug() << __func__;
    if (m_accountsModel->isEmpty()) {
        setSelectedIndex(-1);
        return;
    }
    foreach(Account *account, m_accountsModel->toList()) {
        if (account->accountId() == QMailAccountId(accountId)) {
            setSelectedIndex(m_accountsModel->indexOf(account));
            return;
        }
    }
}

void SenderIdentities::reset()
{
    setSelectedIndex(-1);
    accountsChanged();
}

void SenderIdentities::accountsChanged()
{
    foreach(Account *account, m_accountsModel->toList()) {
        if (account->isPreferredSender()) {
            setSelectedIndex(m_accountsModel->indexOf(account));
            return;
        }
    }
    if (!m_accountsModel->isEmpty()) {
        setSelectedIndex(m_accountsModel->indexOf(m_accountsModel->first()));
    }
}


