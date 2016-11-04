#ifndef ADDRESSBOOKS_H
#define ADDRESSBOOKS_H

#include <QObject>
#include <QmlObjectListModel.h>
#include "AddressBook.h"

class AddressBooks : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY countChanged)
    Q_PROPERTY(QObject *model READ model NOTIFY modelChanged)
public:
    explicit AddressBooks(QObject *parent = 0);

    int count() const { return m_abooks->size(); }
    bool isEmpty() const { return m_abooks->isEmpty(); }
    QObject *model() const;

signals:
    void modelChanged();
    void countChanged();
    // EMit on successful registration
    void addressBookRegistered(QString name);
    // emit when a complete addressbook has been removed
    void addressBookRemoved(QString name);
    // emit when an abook collection has been registered
    void collectionRegistered(QString abook, QString collection);
    // emit when an abook collection has been removed
    void collectionRemoved(QString abook, QString collection);

public slots:
    // plugin wants to register an abook
    void registerAddressBook(const QVariantMap &config);
    // plugin wants to register a collection
    void registerCollection(const QString &abook, const QString &collection);
    // plugin wants to remove a collection
    void removeCollection(const QString &abook, const QString collection);

private:
    QQmlObjectListModel<AddressBook> *m_abooks;

};

#endif // ADDRESSBOOKS_H
