#include "CombinedAddressBookModel.h"

CombinedAddressBookModel::CombinedAddressBookModel(QObject *parent) : RowsJoinerProxy(parent)
{
}

AddressBooks *CombinedAddressBookModel::addressBooks() const
{
    return m_addressBooks;
}

QHash<int, QByteArray> CombinedAddressBookModel::roleNames() const
{
    if (m_books.size() > 0) {
        return static_cast<QAbstractItemModel *>(m_books.at(0)->model())->roleNames();
    } else {
        return QHash<int, QByteArray>();
    }
}

void CombinedAddressBookModel::setAddressBooks(AddressBooks *addressBooks)
{
    if (m_addressBooks == addressBooks)
        return;

    m_addressBooks = addressBooks;
    connect(m_addressBooks->model(), SIGNAL(countChanged(void)), this, SLOT(updateSourceModels()));
    updateSourceModels();
    emit addressBooksChanged(addressBooks);
}

void CombinedAddressBookModel::updateSourceModels()
{
    QQmlObjectListModel<AddressBook> *books = static_cast<QQmlObjectListModel<AddressBook> *>(m_addressBooks->model());
    if (books->isEmpty()) {
        return;
    }
    if (m_books.size() > 0) {
        for (int i = 0; i < m_books.size(); i++) {
            removeSourceModel(static_cast<QAbstractItemModel *>(m_books[i]->model()));
        }
        m_books.clear();
    }
    Q_FOREACH(AddressBook *book, books->toList()) {
        insertSourceModel(static_cast<QAbstractItemModel *>(book->model()));
        m_books.append(book);
    }
}
