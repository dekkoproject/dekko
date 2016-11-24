#include "Collection.h"
#include <QDebug>

Collection::Collection(QObject *parent) : QObject(parent)
{
}

Collection::Collection(QObject *parent, const QString &source, const QString &collection) :
    QObject(parent), m_source(source), m_collection(collection)
{
}

QString Collection::collection() const
{
    return m_collection;
}

QString Collection::source() const
{
    return m_source;
}

QString Collection::startKey() const
{
    return collection();
}

QString Collection::endKey() const
{
    return collection() + "~";
}

void Collection::keyChanged(const QString &key, const QVariant &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}

void Collection::keyRemoved(const QString &key)
{
    Q_UNUSED(key);
}

void Collection::load()
{
}

