#ifndef ADDRESSBOOK_H
#define ADDRESSBOOK_H

#include <QObject>
#include <QmlObjectListModel.h>
#include "Contact.h"
#include "ContactsPlugin.h"

class ContactsPlugin;
class AddressBookBase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY pluginChanged)
    Q_PROPERTY(bool canRead READ canRead NOTIFY pluginChanged)
    Q_PROPERTY(bool canWrite READ canWrite NOTIFY pluginChanged)
    Q_PROPERTY(bool canRemove READ canRemove NOTIFY pluginChanged)
    Q_PROPERTY(QObject *contacts READ model NOTIFY modelChanged)
    QString m_name;
protected:
    explicit AddressBookBase(QObject *parent = 0);
    AddressBookBase(QObject *parent, ContactsPlugin *plugin);
    AddressBookBase(QObject *parent, ContactsPlugin *plugin, const QString &name);
public:
    QObject *model() const;
    QString name() const;
    void setName(const QString &name);
    bool canRead() const;
    bool canWrite() const;
    bool canRemove() const;
    void setPlugin(ContactsPlugin *plugin);

Q_SIGNALS:
    void pluginChanged();
    void modelChanged();

protected:
    QQmlObjectListModel<Contact> *m_model;
    ContactsPlugin *m_plugin;
};

class AddressBook : public AddressBookBase
{
    Q_OBJECT
public:
    explicit AddressBook(QObject *parent = 0);
    AddressBook(QObject *parent, ContactsPlugin *plugin, const QString &name);
    AddressBook(QObject *parent, ContactsPlugin *plugin);

    void save(Contact *contact);
    void remove(Contact *contact);
    void update(Contact *contact);
    QList<Contact *> get(const ContactIdList &uids);

    // QML API
    Q_INVOKABLE void save(QObject *contact);
    Q_INVOKABLE void remove(QObject *contact);
    Q_INVOKABLE void update(QObject *contact);
    Q_INVOKABLE Contact *get(const ContactId &uid);
    Q_INVOKABLE Contact *get(const int &idx);
    Q_INVOKABLE bool merge(const ContactId &c1, const ContactId &c2);
    Q_INVOKABLE void reset();

protected slots:
    // Reimplement these for each plugin;
    virtual void load();
    virtual void reload();
    virtual void clear();
    virtual bool saveContact(Contact *contact);
    virtual bool removeContact(Contact *contact);
    virtual bool updateContact(Contact *contact);
    virtual bool mergeContacts(Contact *c1, Contact *c2);

private slots:
    void init();

};

#endif // ADDRESSBOOK_H
