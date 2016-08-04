#ifndef CONTACTSPLUGIN_H
#define CONTACTSPLUGIN_H

#include <QObject>
#include <QmlObjectListModel.h>
#include "Contact.h"
#include <QPointer>
#include "AddressBook.h"

// CONTACTS PLUGIN
class AddressBook;
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
    virtual AddressBook *create(QObject *parent, const QString &name) = 0;
    virtual bool remove(const QString &name) = 0;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ContactsPlugin::Capabilities)

#endif // CONTACTSPLUGIN_H
