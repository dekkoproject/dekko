#ifndef ABOOKPLUGIN_H
#define ABOOKPLUGIN_H

#include <QObject>
#include <QPointer>
#include <ContactsPlugin.h>
#include <ContactsPluginInterface.h>
#include "ABookTask.h"

class AbookPlugin : public ContactsPlugin
{
    Q_OBJECT
public:
    explicit AbookPlugin(QObject *parent = 0);

public:
    Capabilities capabilities() const;
    ContactListTask *list(const Qt::SortOrder &sortOrder);
    ContactEditTask *add(Contact *contact, ContactEditTask::Action action);
    ContactEditTask *update(Contact *contact, ContactEditTask::Action action);
    ContactEditTask *remove(Contact *contact, ContactEditTask::Action action);
//    ContactImportTask *import(const QByteArray &vcard);
    QStringList addressBooks() const;
    QString addressBookName() const;
    void setAddressbookName(const QString &name);

private:
    QPointer<ABook> m_abook;
    QString m_name;

    // ContactsPlugin interface
public:
};

class contacts_AbookPlugin: public QObject, public IContactsPlugin
{
    Q_OBJECT
    Q_INTERFACES(IContactsPlugin)
    Q_PLUGIN_METADATA(IID "dekko.dekkoproject.contacts.abookplugin")

    // IContactsPlugin interface
public:
    virtual QString pluginName() const override;
    virtual QString pluginDescription() const override;
    virtual ContactsPlugin *create(QObject *parent) override;
    virtual bool hasConfiguredAddressBook() override;

    // IContactsPlugin interface
public:
};

#endif // ABOOKPLUGIN_H
