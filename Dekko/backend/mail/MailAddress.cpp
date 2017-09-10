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
#include "MailAddress.h"
#include <QStringList>
#include <QList>
#include <QDebug>

MailAddress::MailAddress(QObject *parent) : QObject(parent)
{
}

MailAddress::MailAddress(QObject *parent, const QMailAddress &address): QObject(parent),
    m_address(address)
{
    emit addrDataChanged();
}

MailAddress::MailAddress(QObject *parent, const QString &name, const QString &address): QObject(parent),
    m_address(QMailAddress(name, address))
{
    emit addrDataChanged();
}

MailAddress::MailAddress(QObject *parent, const QString &address) : QObject(parent),
    m_address(QMailAddress(address))
{
}

QString MailAddress::name() const
{
    return m_address.name();
}

QString MailAddress::address() const
{
    return m_address.address();
}

QString MailAddress::completeAddress() const
{
    return m_address.toString();
}

QString MailAddress::initials()
{
    QString name = this->name().trimmed();
    if (name.isEmpty() || !name.at(0).isLetter()) {
        // Name is empty so return first character of address. :-/
        if (address().isEmpty()) {
            return QString();
        }
        return QString(address().at(0).toUpper());
    }
    // Intitials string
    QString initials;
    // Now break up the name, we have to set the encoding here as QT_NO_CAST_FROM/TO_ASCII is set
    QStringList parts = name.split(QStringLiteral(" "));
    if (parts.first().at(0).isLetter()) {
        initials += parts.first().at(0).toUpper();
    }
    if (parts.size() > 1) {
        if (parts.last().at(0).isLetter()) {
            initials += parts.last().at(0).toUpper();
        }
    }
    return initials;
}

bool MailAddress::isGroup() const
{
    return m_address.isGroup();
}

bool MailAddress::isNull() const
{
    return m_address.isNull();
}

bool MailAddress::isContact() const
{
    return false; // not implemented yet
}

QString MailAddress::toRfc6068Mailto()
{
    return QStringLiteral("mailto:%1").arg(completeAddress());
}

void MailAddress::setAddress(const QMailAddress &address)
{
    m_address = address;
    emit addrDataChanged();
}

