#include "Contact.h"

Contact::Contact(QObject *parent) : QObject(parent),
   m_address(0), m_emailAddresses(0), m_contactNumbers(0)
{
    m_emailAddresses = new QQmlObjectListModel<ContactEmail>(this);
    m_contactNumbers = new QQmlObjectListModel<ContactPhone>(this);
    emit modelsChanged();
}

QObject *Contact::emailAddresses() const
{
    return m_emailAddresses;
}

QObject *Contact::contactNumbers() const
{
    return m_contactNumbers;
}

int Contact::sortString()
{
    if (!m_firstname.isEmpty()) {
        return (int)m_firstname.toLower().at(0).unicode();
    } else if (!m_emailAddresses->isEmpty()) {
        return (int)m_emailAddresses->at(0)->address().toLower().at(0).unicode();
    } else {
        return -1;
    }
}

QString Contact::section()
{
    if (!m_firstname.isEmpty()) {
        return m_firstname.toUpper().at(0);
    } else if (!m_emailAddresses->isEmpty()) {
        return QString(m_emailAddresses->at(0)->address()).toUpper().at(0);
    } else {
        return tr("Unknown");
    }
}

QString Contact::initials()
{
    QString i;
    if (m_firstname.isEmpty() && m_lastname.isEmpty()) {
        if (hasEmailAddress()) {
            return m_emailAddresses->at(0)->address().at(0).toUpper();
        } else {
            return QString();
        }
    }
    if (m_lastname.isEmpty()) {
        // we might have the full name in the firstname field. i.e the abook plugin
        QStringList parts = m_firstname.split(QChar(' '));
        if (parts.first().at(0).isLetter()) {
            i += parts.first().at(0).toUpper();
        }
        if (parts.size() > 1) {
            if (parts.last().at(0).isLetter()) {
                i += parts.last().at(0).toUpper();
            }
        }
        return i;
    }
    if (m_firstname.at(0).isLetter()) {
        i += m_firstname.at(0).toUpper();
    }
    if (m_lastname.at(0).isLetter()) {
        i += m_lastname.at(0).toUpper();
    }
    return i;
}

void Contact::addContactEmail(ContactEmail *c)
{
    m_emailAddresses->append(c);
}

void Contact::removeContactEmail(const int &idx)
{
    if (idx < 0 || idx > m_emailAddresses->count()) {
        return;
    }
    m_emailAddresses->remove(idx);
}

void Contact::addContactNumber(ContactPhone *c)
{
    m_contactNumbers->append(c);
}

void Contact::addContactAddress(ContactAddress *c)
{
    Q_UNUSED(c);
}

bool Contact::operator ==(const Contact *contact)
{
    return m_uid == contact->uid() && m_addressbook == contact->addressbook();
}

bool Contact::operator ==(const ContactId &uid)
{
    return m_uid == uid;
}

QString ContactPhone::typeToString(ContactPhone::Type t)
{
    switch(t) {
    case Home:
        return tr("Home");
    case Work:
        return tr("Work");
    case Fax:
        return tr("Fax");
    case Mobile:
        return tr("Mobile");
    default:
        return tr("Other");
    }
}
