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
#ifndef SENDERIDENTITIES_H
#define SENDERIDENTITIES_H

#include <QObject>
#include <QmlObjectListModel.h>
#include <Account.h>

class SenderIdentities : public QObject
{
    Q_OBJECT
    // Takes Accounts::model with filter type Accounts::CanSend
    Q_PROPERTY(QObject *accountsModel READ accountsModel WRITE setAccountsModel NOTIFY modelsChanged)
    Q_PROPERTY(QObject *selectedAccount READ selectedAccount NOTIFY selectedIndexChanged)
    Q_PROPERTY(int selectedIndex READ selectedIndex WRITE setSelectedIndex NOTIFY selectedIndexChanged)
public:
    explicit SenderIdentities(QObject *parent = 0);

    int selectedIndex() const;
    QObject *selectedAccount() const;
    QObject *accountsModel() const;
    bool isEmpty();

signals:
    void selectedIndexChanged();
    void modelsChanged();

public slots:
    void setSelectedIndex(int selectedIndex);
    void setAccountsModel(QObject * accountsModel);
    void setSelectedIndexFromAccountId(quint64 accountId);
    void reset();

private slots:
    void accountsChanged(void);

private:
    int m_selectedIndex;
    QQmlObjectListModel<Account> *m_accountsModel;
};

#endif // SENDERIDENTITIES_H
