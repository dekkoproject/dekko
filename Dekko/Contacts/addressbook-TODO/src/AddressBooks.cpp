#include "AddressBooks.h"
#include <QDebug>
#include "Helpers.h"


AddressBooks::AddressBooks(QObject *parent) : QObject(parent)
{
    m_abooks = new QQmlObjectListModel<AddressBook>(this, QByteArray(), QByteArrayLiteral("name"));
    connect(m_abooks, &QQmlObjectListModel<AddressBook>::countChanged, this, &AddressBooks::countChanged);
}

QObject *AddressBooks::model() const {
    return m_abooks;
}

void AddressBooks::registerAddressBook(const QVariantMap &config)
{
    if (config.value("name").isNull()) {
        return;
    }
    m_abooks->append(new AddressBook(Q_NULLPTR, config));
    emit addressBookRegistered(config.value("name").toString());
}

void AddressBooks::registerCollection(const QString &abook, const QString &collection)
{
    if (auto a = m_abooks->getByUid(abook)) {
        if (auto c = a->addCollection(collection)) {
            Q_UNUSED(c);
            emit collectionRegistered(abook, collection);
        }
    }
}

void AddressBooks::removeCollection(const QString &abook, const QString collection)
{

}
