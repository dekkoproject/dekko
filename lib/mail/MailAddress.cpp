#include "MailAddress.h"
#include <QStringList>
#include <QList>

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
    QString name = this->name();
    if (name.isEmpty() || !name.at(0).isLetter()) {
        // Name is empty so return first character of address. :-/
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

void MailAddress::setAddress(const QMailAddress &address)
{
    m_address = address;
    emit addrDataChanged();
}

