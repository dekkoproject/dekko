#ifndef CONTACTSPLUGIN_H
#define CONTACTSPLUGIN_H

#include <QObject>
#include <QmlObjectListModel.h>
#include "Contact.h"
#include <QPointer>
#include "PluginTask.h"

// Contact edit task
class Q_DECL_EXPORT ContactEditTask : public PluginTask
{
    Q_OBJECT
public:
    enum Action {
        Add,
        Update,
        Remove
    };
    ContactEditTask(QObject *parent, Contact *contact, const Action &action):
        PluginTask(parent), contact(contact), action(action){}
signals:
    void contactAdded();
    void contactUpdated();
    void contactRemoved();
    void failed();

protected:
    QPointer<Contact> contact;
    Action action;
};

class Q_DECL_EXPORT ContactListTask : public PluginTask
{
    Q_OBJECT
public:
    ContactListTask(QObject *parent, const Qt::SortOrder &sortOrder):
        PluginTask(parent), sortOrder(sortOrder) {}

    QList<Contact *> list() const { return result; }
signals:
    void listComplete();
    void listFailed();

protected:
    Qt::SortOrder sortOrder;
    QList<Contact *> result;
};

class Q_DECL_EXPORT ContactImportTask : public PluginTask
{
    Q_OBJECT
public:
    explicit ContactImportTask(QObject *parent): PluginTask(parent){}

signals:
    // emit when import is done
    // TODO: import errors
    void importComplete(bool success);
};

// CONTACTS PLUGIN
class Q_DECL_EXPORT ContactsPlugin : public QObject
{
    Q_OBJECT
protected:
    explicit ContactsPlugin(QObject *parent = 0);

public:

    enum Capability {
        ReadContact = 1 << 0,
        WriteContact = 1 << 1,
        RemoveContact = 1 << 2,
        ContactCompletion = 1 << 3,
        ImportFromVCard = 1 << 4,
        ImportFromCSV = 1 << 5,
        ImportFromJCard = 1 << 6,
        ExportToVCard = 1 << 7,
        ExportToJCard = 1 << 8,
        MultipleAddressBooks = 1 << 9
    };

    Q_DECLARE_FLAGS(Capabilities, Capability)
    virtual Capabilities capabilities() const = 0;
    virtual QStringList addressBooks() const = 0;
    virtual QString addressBookName() const = 0;
    virtual void setAddressbookName(const QString &name) = 0;
    virtual ContactListTask* list(const Qt::SortOrder &sortOrder) = 0;
    virtual ContactEditTask* add(Contact *contact, ContactEditTask::Action action = ContactEditTask::Add) = 0;
    virtual ContactEditTask* update(Contact *contact, ContactEditTask::Action action = ContactEditTask::Update) = 0;
    virtual ContactEditTask* remove(Contact *contact, ContactEditTask::Action action = ContactEditTask::Remove) = 0;
//    virtual ContactImportTask* import(const QByteArray &vcard) = 0;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(ContactsPlugin::Capabilities)

#endif // CONTACTSPLUGIN_H
