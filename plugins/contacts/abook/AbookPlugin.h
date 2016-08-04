#ifndef ABOOKPLUGIN_H
#define ABOOKPLUGIN_H

#include <QObject>
#include <QPointer>
#include <ContactsPlugin.h>
#include <ContactsPluginInterface.h>
#include "ABookAddressBook.h"

class AbookPlugin : public ContactsPlugin
{
    Q_OBJECT
public:
    explicit AbookPlugin(QObject *parent = 0);

    Capabilities capabilities() const;
    QStringList addressBooks() const;
    AddressBook *create(QObject *parent, const QString &name);
    bool remove(const QString &name);
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
