#include "AbookPlugin.h"
#include <QFile>
#include <Paths.h>


AbookPlugin::AbookPlugin(QObject *parent) : ContactsPlugin(parent)
{
    m_abook = new ABook(this);
}

ContactsPlugin::Capabilities AbookPlugin::capabilities() const
{
    return ContactsPlugin::ReadContact | ContactsPlugin::WriteContact |
            ContactsPlugin::RemoveContact | ContactsPlugin::ContactCompletion |
            ContactsPlugin::ImportFromVCard;
}

ContactListTask *AbookPlugin::list(const Qt::SortOrder &sortOrder)
{
    ABookListTask *task = new ABookListTask(this, Qt::AscendingOrder);
    task->setABook(m_abook);
    task->setDeleteWhenDone(false);
    return task;
}

ContactEditTask *AbookPlugin::add(Contact *contact, ContactEditTask::Action action)
{
    ABookEditTask *task = new ABookEditTask(this, contact, action);
    task->setABook(m_abook);
    task->setDeleteWhenDone(true);
    return task;
}

ContactEditTask *AbookPlugin::update(Contact *contact, ContactEditTask::Action action)
{
    ABookEditTask *task = new ABookEditTask(this, contact, action);
    task->setABook(m_abook);
    task->setDeleteWhenDone(true);
    return task;
}

ContactEditTask *AbookPlugin::remove(Contact *contact, ContactEditTask::Action action)
{
    ABookEditTask *task = new ABookEditTask(this, contact, action);
    task->setABook(m_abook);
    task->setDeleteWhenDone(true);
    return task;
}

QStringList AbookPlugin::addressBooks() const
{
    // we don't support multi addressbooks and the default name is abook so use that.
    return QStringList() << QStringLiteral("abook");
}

QString AbookPlugin::addressBookName() const
{
    return m_name;
}

void AbookPlugin::setAddressbookName(const QString &name)
{
    m_name = name;
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
