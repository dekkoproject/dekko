#include "ABookAddressBook.h"

ABookAddressBook::ABookAddressBook(QObject *parent) : AddressBook(parent), m_abook(0)
{
    m_abook = new ABook(this);
}

ABookAddressBook::ABookAddressBook(QObject *parent, ContactsPlugin *plugin, const QString &name):
    AddressBook(parent, plugin,name), m_abook(0)
{
    m_abook = new ABook(this);
}

ABookAddressBook::ABookAddressBook(QObject *parent, ContactsPlugin *plugin):
    AddressBook(parent, plugin), m_abook(0)
{
    m_abook = new ABook(this);
}

void ABookAddressBook::load()
{
    ContactIdList contactList = m_abook->list();
    if (contactList.isEmpty()) {
        return;
    }
    Q_FOREACH(const ContactId &uid, contactList) {
        Contact *c = m_abook->get(uid);
        if (c != Q_NULLPTR) {
            m_model->append(c);
        }
    }
}

void ABookAddressBook::reload()
{
    clear();
    load();
}

void ABookAddressBook::clear()
{
    m_model->clear();
}

bool ABookAddressBook::saveContact(Contact *contact)
{
    if (!contact->uid().isEmpty()) {
        return updateContact(contact);
    } else {
        const bool res = m_abook->add(contact);
        if (res) {
            m_model->append(contact);
        }
        return res;
    }
}

bool ABookAddressBook::removeContact(Contact *contact)
{
    if (m_model->contains(contact)) {
        m_abook->remove(contact);
        m_model->remove(contact);
        reload();
    }
}

bool ABookAddressBook::updateContact(Contact *contact)
{
    if (contact->uid().isEmpty()) {
        return saveContact(contact);
    } else {
        return m_abook->update(contact);
    }
}

bool ABookAddressBook::mergeContacts(Contact *c1, Contact *c2)
{

}
