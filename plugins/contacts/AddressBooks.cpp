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
                p->setAddressbookName(name);
                AddressBook *book = new AddressBook(this, p);
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
