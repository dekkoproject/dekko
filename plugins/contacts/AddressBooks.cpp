#include "AddressBooks.h"
#include <QDebug>
#include "Helpers.h"
AddressBooks::AddressBooks(QObject *parent) : QObject(parent), m_books(0),
    loader(0)
{
    m_books = new QQmlObjectListModel<AddressBook>(this);
    loader = new ContactsPluginLoader(this);
    DELAY_CALL_NOARG(this, init);
}

QObject *AddressBooks::model() const
{
    return m_books;
}

QObject *AddressBooks::combinedModel() const
{
    return m_combined;
}

void AddressBooks::saveContact(QObject *contact)
{
    Contact *c = qobject_cast<Contact *>(contact);
    if (c) {
        AddressBook *ab = findAddressBook(c->addressbook());
        if (ab) {
            ab->save(c);
        }
    }
}

void AddressBooks::removeContact(QObject *contact)
{
    Contact *c = qobject_cast<Contact *>(contact);
    if (c) {
        AddressBook *ab = findAddressBook(c->addressbook());
        if (ab) {
            ab->remove(c);
        }
    }
}

void AddressBooks::init()
{
    QMap<QString, IContactsPlugin*> plugins = loader->contactsPlugins();
    Q_FOREACH(auto &plugin, loader->contactsPlugins()) {
        if (plugin->hasConfiguredAddressBook()) {
            // Plugins can support multiple addressbooks
            // now we could check for the ContactsPlugin::MultipleAddressBooks capability
            // but instead let's just iterate the addressbook names and create an
            // AddressBook for each name.
            ContactsPlugin *tmp = plugin->create(this);
            bool tmpUsed = false; // saves wasting the above pointer;
            qDebug() << "Plugin:" << plugin->pluginName() << ". Addressbooks:" << tmp->addressBooks();
            Q_FOREACH(const QString &name, tmp->addressBooks()) {
                ContactsPlugin *p = 0;
                if (!tmpUsed) {
                    p = tmp;
                    tmpUsed = true;
                } else {
                    p = plugin->create(this);
                }
                AddressBook *book = p->create(this, name);
                book->setObjectName(QStringLiteral("addressbook-%1").arg(name));
                p->setParent(book);
                m_books->append(book);
            }
        } else {
            qDebug() << "No configured addressbooks for" << plugin->pluginName();
        }
    }
    if (!m_books->isEmpty()) {
        DELAY_EMIT_NOARG(this, pluginsLoaded);
    }
}

AddressBook *AddressBooks::findAddressBook(const QString &name)
{
    foreach(AddressBook *ab, m_books->toList()) {
        if (ab->name() == name) {
            return ab;
        }
    }
    return Q_NULLPTR;
}
