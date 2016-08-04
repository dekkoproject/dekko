#include "AddressBook.h"
#include <QDebug>
#include <QTimer>
#include <Helpers.h>

AddressBookBase::AddressBookBase(QObject *parent) : QObject(parent),
    m_model(0), m_plugin(0)
{
    m_model = new QQmlObjectListModel<Contact>(this);
    emit modelChanged();
}

AddressBookBase::AddressBookBase(QObject *parent, ContactsPlugin *plugin) : QObject(parent),
    m_model(0), m_plugin(0)
{
    m_model = new QQmlObjectListModel<Contact>(this, QByteArray(), QByteArrayLiteral("uid"));
    emit modelChanged();
    setPlugin(plugin);
}

AddressBookBase::AddressBookBase(QObject *parent, ContactsPlugin *plugin, const QString &name) : QObject(parent),
    m_model(0), m_plugin(0)
{
    m_model = new QQmlObjectListModel<Contact>(this, QByteArray(), QByteArrayLiteral("uid"));
    emit modelChanged();
    setPlugin(plugin);
    setName(name);
}

QObject *AddressBookBase::model() const
{
    return m_model;
}

QString AddressBookBase::name() const
{
    return m_name;
}

void AddressBookBase::setName(const QString &name)
{
    m_name = name;
}

bool AddressBookBase::canRead() const
{
    return (m_plugin->capabilities() & ContactsPlugin::ReadContact);
}

bool AddressBookBase::canWrite() const
{
    return (m_plugin->capabilities() & ContactsPlugin::WriteContact);
}

bool AddressBookBase::canRemove() const
{
    return (m_plugin->capabilities() & ContactsPlugin::RemoveContact);
}

void AddressBookBase::setPlugin(ContactsPlugin *plugin) {
    m_plugin = plugin;
    emit pluginChanged();
}

AddressBook::AddressBook(QObject *parent) : AddressBookBase(parent)
{
}
AddressBook::AddressBook(QObject *parent, ContactsPlugin *plugin, const QString &name)
    : AddressBookBase(parent, plugin, name)
{
    DELAY_CALL_NOARG(this, init);
}

AddressBook::AddressBook(QObject *parent, ContactsPlugin *plugin) :
    AddressBookBase(parent, plugin)
{
    DELAY_CALL_NOARG(this, init);
}

void AddressBook::save(QObject *contact)
{
    Contact *c = qobject_cast<Contact *>(contact);
    if (c) {
        save(c);
    }
}

void AddressBook::save(Contact *contact)
{
    if (canWrite()) {
        saveContact(contact);
    } else {
        qWarning() << "Addressbook doesn't support writing";
    }
}

void AddressBook::remove(QObject *contact)
{
    Contact *c = qobject_cast<Contact *>(contact);
    if (c) {
        remove(c);
    }
}

void AddressBook::remove(Contact *contact)
{
    if (canRemove()) {
        if (m_model->contains(contact)) {
            if (removeContact(contact)) {
                m_model->remove(contact);
            }
        } else {
            qWarning() << "Model doesn't contain contact";
            removeContact(contact);
        }
    } else {
        qDebug() << "Contact removal not supported";
    }
}

void AddressBook::update(QObject *contact)
{
    Contact *c = qobject_cast<Contact *>(contact);
    if (c) {
        update(c);
    }
}

void AddressBook::update(Contact *contact)
{
    if (canWrite() && m_model->contains(contact)) {
        updateContact(contact);
    }
}

Contact *AddressBook::get(const ContactId &uid)
{
    if (canRead()) {
        return m_model->getByUid(uid);
    } else {
        return Q_NULLPTR;
    }
}

Contact *AddressBook::get(const int &idx)
{
    if (canRead()) {
        return m_model->at(idx);
    } else {
        return Q_NULLPTR;
    }
}

QList<Contact *> AddressBook::get(const ContactIdList &uids)
{
    QList<Contact *> res;
    Q_FOREACH(const ContactId &uid, uids) {
        Contact *c = get(uid);
        if (c != Q_NULLPTR) {
            res << c;
        }
    }
    return res;
}

bool AddressBook::merge(const ContactId &c1, const ContactId &c2)
{
    if (c1 == c2) {
        qDebug() << "Cannot merge contact into itself";
        return false;
    }
    if (canWrite()) {
        Contact *ct1 = m_model->getByUid(c1);
        Contact *ct2 = m_model->getByUid(c2);

        if (ct1 && ct2) {
            return mergeContacts(ct1, ct2);
        }
    }
    qDebug() << "MERGE FAILED INVALID CONTACT";
    return false;
}

void AddressBook::reset()
{
    DELAY_CALL_NOARG(this, reload);
}

void AddressBook::load()
{
    qWarning() << "Not implemented here";
}

void AddressBook::reload()
{
    qWarning() << "Not implemented here";
}

void AddressBook::clear()
{
    qWarning() << "Not implemented here";
}

bool AddressBook::saveContact(Contact *contact)
{
    Q_UNUSED(contact);
    qWarning() << "Not implemented here";
    return false;
}

bool AddressBook::removeContact(Contact *contact)
{
    Q_UNUSED(contact);
    qWarning() << "Not implemented here";
    return false;
}

bool AddressBook::updateContact(Contact *contact)
{
    Q_UNUSED(contact);
    qWarning() << "Not implemented here";
    return false;
}

bool AddressBook::mergeContacts(Contact *c1, Contact *c2)
{
    Q_UNUSED(c1);
    Q_UNUSED(c2);
    qWarning() << "Not implemented here";
    return false;
}

void AddressBook::init()
{
    qDebug() << "Loading addressbook for the first time";
    DELAY_CALL_NOARG(this, load);
}
