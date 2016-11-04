#include "AddressBook.h"


AddressBook::AddressBook(QObject *parent) : QObject(parent),
    m_collections(Q_NULLPTR)
{
    m_collections = new QQmlObjectListModel<Collection>(this, QByteArrayLiteral("name"));
    connect(m_collections, &QQmlObjectListModel<Collection>::countChanged, this, &AddressBook::collectionsChanged);
}

AddressBook::AddressBook(QObject *parent, const QVariantMap &config) : QObject(parent),
    m_collections(Q_NULLPTR)
{
    setConfig(config);
    m_collections = new QQmlObjectListModel<Collection>(this, QByteArrayLiteral("name"));
    connect(m_collections, &QQmlObjectListModel<Collection>::countChanged, this, &AddressBook::collectionsChanged);
}

QString AddressBook::name() const
{
    return m_config.value("name", QString()).toString();
}

QString AddressBook::displayName() const
{
    return  m_config.value("displayName", QString()).toString();
}

QString AddressBook::path() const
{
    return m_config.value("path", QString()).toString();
}

QString AddressBook::settings() const
{
    return m_config.value("settings", QString()).toString();
}

bool AddressBook::canRead() const
{
    return m_options.value("canRead", false).toBool();
}

bool AddressBook::canWrite() const
{
    return m_options.value("canWrite", false).toBool();
}

bool AddressBook::canDelete() const
{
    return m_options.value("canDelete", false).toBool();
}

bool AddressBook::canSync() const
{
    return m_options.value("canSync", false).toBool();
}

bool AddressBook::multipleCollections() const
{
    return m_options.value("multipleCollections", false).toBool();
}

void AddressBook::setConfig(const QVariantMap &config)
{
    m_config = config;
    m_options = config.value("options").toMap();
}

QObject *AddressBook::collections() const
{
    return m_collections;
}

Collection *AddressBook::addCollection(const QString &collection)
{
    if (collection.isEmpty()) {
        return Q_NULLPTR;
    }
    auto c = new Collection(Q_NULLPTR, path(), collection);
    m_collections->append(c);
    return c;
}

void AddressBook::handleKeyValueChanged(const QString &key, const QVariant &value)
{
    // Strip out the collection name from the key
    QStringList splitKeys = key.split('/', QString::SkipEmptyParts);
    if (auto collection = m_collections->getByUid(splitKeys.first())) {
        collection->keyChanged(key, value);
    } else {
        // unknown collection :-/
    }
}

void AddressBook::handleKeyRemoved(const QString &key)
{
    // Strip out the collection name from the key
    QStringList splitKeys = key.split('/', QString::SkipEmptyParts);
    if (auto collection = m_collections->getByUid(splitKeys.first())) {
        collection->keyRemoved(key);
    } else {
        // unknown collection :-/
    }
}
