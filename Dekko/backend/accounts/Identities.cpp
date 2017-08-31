#include "Identities.h"
#include <SnapStandardPaths.h>
#include <QVariant>
#include <MazDBBatch.h>

namespace {
    static const QString prefix = QStringLiteral("/identity/");
    static const QString metaKey = QStringLiteral("/meta");
}

Identities::Identities(QObject *parent) : QObject(parent),
    m_db(Q_NULLPTR), m_defaultIdentity(0)
{
    m_sourcePath = SnapStandardPaths::writableLocation(SnapStandardPaths::AppConfigLocation) + QStringLiteral("/mazdb/identities.db");
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
        metaInfo["default"] = count;
        emit defaultIdentityChanged(count);
    } else {
        metaInfo = m_db->get(metaKey).toMap();
        count = metaInfo.value("count").toInt();
        metaInfo["count"] = ++count;
    }
    batch->put(metaKey, metaInfo);

    identity["id"] = count;

    batch->put(key(QString::number(count)), identity);
    bool result = batch->write();
    if (result)
        emit identitiesChanged();
    return result;
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
    bool result = m_db->put(key(QString::number(map["id"].toInt())), map);
    if (result)
        emit identitiesChanged();
    return result;
}

bool Identities::remove(const int &id)
{
    QString k = key(QString::number(id));
    if (!m_db->get(k).isValid()) {
        return false;
    }
    bool result = m_db->del(k);
    if (result)
        emit identitiesChanged();
    return result;
}

bool Identities::remove(const QVariantMap &map)
{
    return remove(map["id"].toInt());
}

bool Identities::removeAccount(const int &id)
{
    auto batch = m_db->batch();
    MazCallBack func = [=](QString key, QVariant val) {
        QVariantMap identity = val.toMap();
        if (identity["parentId"].toInt() == id) {
            batch->del(key);
        }
        return true;
    };
    m_db->readStream(func, prefix);
    bool result = batch->write();
    if (result)
        emit identitiesChanged();
    return result;
}

QVariantMap Identities::get(const int &id)
{
    QString k = key(QString::number(id));
    if (!m_db->get(k).isValid()) {
        return QVariantMap();
    }
    return m_db->get(k).toMap();
}

IdentityList Identities::getAllForAccount(const int &id)
{
    QList<QVariantMap> identities;
    MazValueCallBack func = [this, &identities, &id](QVariant val) {
        const QVariantMap identity = val.toMap();
        if (identity["parentId"].toInt() == id) {
            identities.append(identity);
        }
        return true;
    };
    m_db->valueStream(func, prefix);
    return identities;
}

int Identities::defaultIdentity() const
{
    return m_db->get(metaKey, 0).toMap().value("default").toInt();
}

int Identities::determineBestIdentityFromMessage(const QMailMessage &message)
{

    auto identities = getAllForAccount(message.parentAccountId().toULongLong());

    auto identityInRecipients = [=](QMailAddressList list, const QString &match) -> bool {
        for (const auto &address : list) {
            if (address.address() == match) {
                return true;
            }
        }
        return false;
    };

    for (const auto &i : identities) {
        const QString email = i["email"].toString();
        if (message.from().address() == email || // we also check "from" here because it could be a draft message
                identityInRecipients(message.to(), email) ||
                identityInRecipients(message.cc(), email) ||
                identityInRecipients(message.bcc(), email)) {
            return i["id"].toInt();
        }
    }

    return -1;
}

void Identities::setDefaultIdentity(int defaultIdentity)
{
    QVariantMap meta = m_db->get(metaKey).toMap();
    meta["default"] = defaultIdentity;
    m_db->putSync(metaKey, meta);
    emit defaultIdentityChanged(defaultIdentity);
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
