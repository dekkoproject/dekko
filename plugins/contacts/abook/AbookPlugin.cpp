#include "AbookPlugin.h"
#include <QFile>
#include <Paths.h>


AbookPlugin::AbookPlugin(QObject *parent) : ContactsPlugin(parent)
{
}

ContactsPlugin::Capabilities AbookPlugin::capabilities() const
{
    return ContactsPlugin::ReadContact | ContactsPlugin::WriteContact |
            ContactsPlugin::RemoveContact | ContactsPlugin::ContactCompletion |
            ContactsPlugin::ImportFromVCard;
}

QStringList AbookPlugin::addressBooks() const
{
    // we don't support multi addressbooks and the default name is abook so use that.
    return QStringList() << QStringLiteral("abook");
}

AddressBook *AbookPlugin::create(QObject *parent, const QString &name)
{
    if (!name.isEmpty()) {
        return new ABookAddressBook(parent, this, name);
    }
    return Q_NULLPTR;
}

bool AbookPlugin::remove(const QString &name)
{
    Q_UNUSED(name);
    return false;
}

QString contacts_AbookPlugin::pluginName() const
{
    return QStringLiteral("abook");
}

QString contacts_AbookPlugin::pluginDescription() const
{
    return tr("abook format addressbook");
}

ContactsPlugin *contacts_AbookPlugin::create(QObject *parent)
{
    return new AbookPlugin(parent);
}

bool contacts_AbookPlugin::hasConfiguredAddressBook()
{
    // simple enough :-)
    QString homePath = Paths::homeLocation().append(QStringLiteral("/.abook/addressbook"));
    if (QFile::exists(homePath)) {
        return true;
    }
    QString path = Paths::dataLocationForFile(QStringLiteral("/contacts/.abook/addressbook"));
    qDebug() << "ABOOK HAS CONFIGURED AT: " << path;
    return QFile::exists(path);
}
