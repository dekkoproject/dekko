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
#ifndef SENDERIDENTITIES_H
#define SENDERIDENTITIES_H

#include <QObject>
#include <QmlObjectListModel.h>
#include <Account.h>
#include <Identities.h>
#include <Helpers.h>

class IdentityWrapper : public QObject
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY(Account*, account)
    QML_WRITABLE_PROPERTY(Identity*, identity)
public:
    explicit IdentityWrapper(QObject *parent = Q_NULLPTR) :
        QObject(parent), m_account(0), m_identity(0) {}
};

class SenderIdentities : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *model READ model NOTIFY modelsChanged)
    // Takes Accounts::model with filter type Accounts::CanSend
    Q_PROPERTY(QObject *accountsModel READ accountsModel WRITE setAccountsModel NOTIFY modelsChanged)
    Q_PROPERTY(Identities *accountIdentities READ identities WRITE setIdentities NOTIFY modelsChanged)
    Q_PROPERTY(QObject *selectedAccount READ selectedAccount NOTIFY selectedIndexChanged)
    Q_PROPERTY(int selectedIndex READ selectedIndex WRITE setSelectedIndex NOTIFY selectedIndexChanged)
public:
    explicit SenderIdentities(QObject *parent = 0);

    QObject *model() const;
    int selectedIndex() const;
    QObject *selectedAccount() const;
    QObject *accountsModel() const;
    bool isEmpty();
    bool hasAddress(const QString &address);

    Identities *identities() const;

    int determinePreferredIdentity(const QMailMessage &message);

signals:
    void selectedIndexChanged();
    void modelsChanged();

public slots:
    void setSelectedIndex(int selectedIndex);
    void setAccountsModel(QObject *accountsModel);
    void setSelectedIndexFromAccountId(quint64 accountId);
    void reset();
    void setIdentities(Identities *accountIdentities);

private slots:
    void accountsChanged(void);

private:
    int findIndexOf(const quint64 &id);
    IdentityWrapper *createIdentityFromAccount(Account *account);
    IdentityWrapper *createIdentity(Account *account, const QVariantMap &identity);
    int m_selectedIndex;
    QQmlObjectListModel<Account> *m_accountsModel;
    Identities *m_accountIdentities;
    QQmlObjectListModel<IdentityWrapper> *m_model;
};

#endif // SENDERIDENTITIES_H
