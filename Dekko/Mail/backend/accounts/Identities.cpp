#include "Identities.h"
#include <QStandardPaths>
#include <MazDBBatch.h>

namespace {
    static const QString prefix = QStringLiteral("/identity/");
    static const QString metaKey = QStringLiteral("/meta");
}

Identities::Identities(QObject *parent) : QObject(parent),
    m_db(Q_NULLPTR)
{
    m_sourcePath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + QStringLiteral("/mazdb/identities.db");
    emit sourcePathChanged(m_sourcePath);
    m_db = new MazDB(m_sourcePath, this);
}

bool Identities::add(const QVariantMap &map)
{
    if (!map.contains(QStringLiteral("parentId"))) {
        emit error(InvalidParent);
        m_errorString = "Missing parent accountId";
        return false;
    }

    QString parentId = map.value(QStringLiteral("parentId")).toString();

    QVariantMap metaInfo;
    QVariantMap identity = map;

    auto batch = m_db->batch();
    int count = 1;
    if (!m_db->get(metaKey).isValid()) {
        // this is a new account identity
        metaInfo["count"] = count;
    } else {
        metaInfo = m_db->get(metaKey).toMap();
        count = metaInfo.value("count").toInt();
        metaInfo["count"] = ++count;
    }
    batch->put(metaKey, metaInfo);

    identity["id"] = count;

    batch->put(key(QString::number(count)), identity);
    return batch->write();
}

bool Identities::update(const QVariantMap &map)
{
    if (!map.contains(QStringLiteral("parentId"))) {
        emit error(InvalidParent);
        m_errorString = "Missing parent accountId";
        return false;
    }
    if (!map.contains(QStringLiteral("id"))) {
        emit error(InvalidId);
        m_errorString = "Missing identity id";
        return false;
    }
    return m_db->put(key(QString::number(map["id"].toInt())), map);
}

bool Identities::remove(const int &id)
{
    QString k = key(QString::number(id));
    if (!m_db->get(k).isValid()) {
        return false;
    }
    return m_db->del(k);
}

bool Identities::remove(const QVariantMap &map)
{
    return remove(map["id"].toInt());
}

QVariantMap Identities::get(const int &id)
{
    QString k = key(QString::number(id));
    if (!m_db->get(k).isValid()) {
        return QVariantMap();
    }
    return m_db->get(k).toMap();
}

QString Identities::key(const QString &k)
{
    QString result(prefix);

    if (k.startsWith('/')) {
        result.append(k.mid(1));
    } else {
        result.append(k);
    }
    return result;
}
