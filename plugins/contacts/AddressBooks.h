#ifndef ADDRESSBOOKS_H
#define ADDRESSBOOKS_H

#include <QObject>
#include <QmlObjectListModel.h>
#include <RowsJoinerProxy.h>
#include "AddressBook.h"
#include "ContactsPlugin.h"
#include "ContactsPluginLoader.h"

class CombinedAddressBooks;
class AddressBooks : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *model READ model NOTIFY modelsChanged)
    Q_PROPERTY(QObject *combinedModel READ combinedModel NOTIFY combinedModelChanged)
public:
    explicit AddressBooks(QObject *parent = 0);

    QObject *model() const;
    QObject *combinedModel() const;
    Q_INVOKABLE void saveContact(QObject *contact);
    Q_INVOKABLE void removeContact(QObject *contact);

Q_SIGNALS:
    void modelsChanged();
    void combinedModelChanged();
    void pluginsLoaded();

private slots:
    void init();

private:
    AddressBook *findAddressBook(const QString &name);
    QQmlObjectListModel<AddressBook> *m_books;
    ContactsPluginLoader *loader;
    CombinedAddressBooks *m_combined;
};

class CombinedAddressBooks : public RowsJoinerProxy
{
    Q_OBJECT
public:
    explicit CombinedAddressBooks(QObject *parent = 0) : RowsJoinerProxy(parent), m_abooks(0){}
    QHash<int, QByteArray> roleNames() const
    {
        if (m_abooks && !m_abooks->isEmpty())
            return static_cast<QAbstractItemModel *>(m_abooks->first()->model())->roleNames();
        else
            return QHash<int, QByteArray>();
    }
    void setModel(QQmlObjectListModel<AddressBook> *model) {
        m_abooks = model;
    }

private:
    QQmlObjectListModel<AddressBook> *m_abooks;
};

#endif // ADDRESSBOOKS_H
