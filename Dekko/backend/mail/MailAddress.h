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
#ifndef MAILADDRESS_H
#define MAILADDRESS_H

#include <QObject>
#include <QCryptographicHash>
#include "qmailaddress.h"

class MailAddress : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY addrDataChanged)
    Q_PROPERTY(QString address READ address NOTIFY addrDataChanged)
    Q_PROPERTY(QString completeAddress READ completeAddress NOTIFY addrDataChanged)
    Q_PROPERTY(QString initials READ initials NOTIFY addrDataChanged)
    Q_PROPERTY(bool isGroup READ isGroup NOTIFY addrDataChanged)
    Q_PROPERTY(bool isNull READ isNull NOTIFY addrDataChanged)

    Q_PROPERTY(bool isContact READ isContact NOTIFY addrDataChanged)
public:
    explicit MailAddress(QObject *parent = 0);
    MailAddress(QObject *parent, const QMailAddress &address);
    MailAddress(QObject *parent, const QString &name, const QString &address);
    MailAddress(QObject *parent, const QString &address);

    QString name() const;
    QString address() const;
    QString completeAddress() const;
    QString initials();
    bool isGroup() const;
    bool isNull() const;
    bool isContact() const;

    Q_INVOKABLE QString toRfc6068Mailto();

    QMailAddress qMailAddress() const { return m_address; }

signals:
    void addrDataChanged();

public slots:
    void setAddress(const QMailAddress &address);

private:
    QMailAddress m_address;
};

#endif // MAILADDRESS_H
