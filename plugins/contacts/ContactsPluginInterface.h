#ifndef CONTACTS_PLUGIN_INTERFACE
#define CONTACTS_PLUGIN_INTERFACE

#include <QObject>
#include <QtPlugin>

class ContactsPlugin;
class IContactsPlugin
{
public:
    virtual QString pluginName() const = 0;
    virtual QString pluginDescription() const = 0;
    // Indicates if a plugin has already been configured
    // and is ok to create it without user initiation.
    virtual bool hasConfiguredAddressBook() = 0;
    virtual ContactsPlugin *create(QObject *parent) = 0;
};

Q_DECLARE_INTERFACE(IContactsPlugin, "IContactsPlugin")

#endif //TROJITAPLUGIN_INTERFACE
