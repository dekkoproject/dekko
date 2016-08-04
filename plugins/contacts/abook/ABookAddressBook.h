#ifndef ABOOKADDRESSBOOK_H
#define ABOOKADDRESSBOOK_H

#include <QObject>
#include "ABook.h"
#include <AddressBook.h>

class ABookAddressBook : public AddressBook
{
    Q_OBJECT
public:
    explicit ABookAddressBook(QObject *parent = 0);
    ABookAddressBook(QObject *parent, ContactsPlugin *plugin);
    ABookAddressBook(QObject *parent, ContactsPlugin *plugin, const QString &name);

    // AddressBook interface
protected slots:
    virtual void load() override;
    virtual void reload() override;
    virtual void clear() override;
    virtual bool saveContact(Contact *contact) override;
    virtual bool removeContact(Contact *contact) override;
    virtual bool updateContact(Contact *contact) override;
    virtual bool mergeContacts(Contact *c1, Contact *c2) override;

private:
    ABook *m_abook;
};

#endif // ABOOKADDRESSBOOK_H
