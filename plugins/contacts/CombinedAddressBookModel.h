#ifndef COMBINEDADDRESSBOOKMODEL_H
#define COMBINEDADDRESSBOOKMODEL_H

#include <QObject>
#include <RowsJoinerProxy.h>
#include <QmlObjectListModel.h>
#include "AddressBooks.h"

class CombinedAddressBookModel : public RowsJoinerProxy
{
    Q_OBJECT
    Q_PROPERTY(AddressBooks *addressBooks READ addressBooks WRITE setAddressBooks NOTIFY addressBooksChanged)
public:
    explicit CombinedAddressBookModel(QObject *parent = 0);
    AddressBooks *addressBooks() const;
    QHash<int, QByteArray> roleNames() const;

Q_SIGNALS:
    void addressBooksChanged(AddressBooks *addressBooks);

public slots:

    void setAddressBooks(AddressBooks *addressBooks);

private slots:
    void updateSourceModels();

private:
    AddressBooks *m_addressBooks;
    QList<AddressBook *> m_books;
};

#endif // COMBINEDADDRESSBOOKMODEL_H
