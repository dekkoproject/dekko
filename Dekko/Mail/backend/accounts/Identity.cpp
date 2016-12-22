#include "Identity.h"
#include <QDebug>

namespace {
    static const QString accountId = QStringLiteral("id");
    static const QString parentId = QStringLiteral("parentId");
    static const QString name = QStringLiteral("name");
    static const QString email = QStringLiteral("email");
    static const QString replyTo = QStringLiteral("replyTo");
    static const QString signature = QStringLiteral("signature");
}

Identity::Identity(QObject *parent) : QObject(parent),
    m_accountId(0), m_parentId(0), m_isDefault(false)
{
}

Identity *Identity::fromMap(QObject *p, const QVariantMap &map)
{
    if (map.isEmpty()) {
        return Q_NULLPTR;
    }
    Identity *identity = new Identity(p);
    if (map.contains(accountId)) {
        identity->set_accountId(map.value(accountId).toUInt());
    }
    if (map.contains(parentId)) {
        identity->set_parentId(map.value(parentId).toUInt());
    }
    if (map.contains(name)) {
        identity->set_name(map.value(name).toString());
    }
    if (map.contains(email)) {
        identity->set_email(map.value(email).toString());
    }
    if (map.contains(replyTo)) {
        identity->set_replyTo(map.value(replyTo).toString());
    }
    if (map.contains(signature)) {
        identity->set_signature(map.value(signature).toString());
    }
    if (map.contains(QStringLiteral("default"))) {
        identity->set_isDefault(map.value(QStringLiteral("default")).toBool());
    }
    return identity;
}

QVariantMap Identity::toMap()
{
    QVariantMap map;
    map[accountId] = get_accountId();
    map[parentId] = get_parentId();
    map[name] = get_name();
    map[email] = get_email();
    map[replyTo] = get_replyTo();
    map[signature] = get_signature();
    map[QStringLiteral("default")] = get_isDefault();
    return map;
}
