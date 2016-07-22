#ifndef ADDRESSBOOK_H
#define ADDRESSBOOK_H

#include <QObject>
#include <QmlObjectListModel.h>
#include "Contact.h"
#include "ContactsPlugin.h"

class AddressBook : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY pluginChanged)
    Q_PROPERTY(bool canRead READ canRead NOTIFY pluginChanged)
    Q_PROPERTY(bool canWrite READ canWrite NOTIFY pluginChanged)
    Q_PROPERTY(bool canRemove READ canRemove NOTIFY pluginChanged)
    Q_PROPERTY(QObject *contacts READ model NOTIFY modelChanged)

public:
    explicit AddressBook(QObject *parent = 0);
    AddressBook(QObject *parent, ContactsPlugin *plugin);

    QString name() const;
    bool canRead() const;
    bool canWrite() const;
    bool canRemove() const;

    QObject *model() const { return m_model; }
signals:
    void pluginChanged();
    void modelChanged();

private slots:
    void init();
    void load();
    void reload();
    void clear();

private:
    ContactsPlugin *m_plugin;
    QQmlObjectListModel<Contact> *m_model;
    ContactListTask *m_listTask;
};

#endif // ADDRESSBOOK_H
