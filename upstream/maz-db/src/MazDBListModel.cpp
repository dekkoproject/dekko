#include "MazDBListModel.h"
#include "MazDB.h"
#include <QDebug>
#include <QString>
#include <QStringList>
#include <qqmlinfo.h>
#include "QJsEngineShim.h"

MazDBListModel::MazDBListModel(QObject *parent) : QAbstractListModel(parent),
    m_db(Q_NULLPTR), m_filterPolicy(Contains)
{
    m_db = new MazDB(this);
    connect(m_db, &MazDB::keyValueChanged, this, &MazDBListModel::handleKeyValueChange);
    connect(m_db, &MazDB::filenameChanged, this, &MazDBListModel::sourceChanged);
    connect(m_db, &MazDB::keyValueRemoved, this, &MazDBListModel::handleKeyValueRemoved);
}

int MazDBListModel::count() const
{
    return m_keys.size();
}

Range *MazDBListModel::range()
{
    return &m_range;
}

QString MazDBListModel::source() const
{
    return m_db->filename();
}

QString MazDBListModel::filter() const
{
    return m_filter;
}

QHash<int, QByteArray> MazDBListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(KeyRole, QByteArrayLiteral("key"));
    roles.insert(ValueRole, QByteArrayLiteral("value"));
    if (!m_customRoles.isEmpty()) {
        int i = ValueRole + 1;
        for (const QString &role : m_customRoles) {
            roles.insert(i, role.toUtf8());
            ++i;
        }
    }
    return roles;
}

QVariant MazDBListModel::data(const QModelIndex &index, int role) const
{
    QVariant value;
    const int row = index.row();
    if (row >= 0 && row < count()) {
        QString key = m_keys.value(row);
        switch(role) {
        case KeyRole:
            value = key;
            break;
        case ValueRole:
            value = m_db->get(key);
            break;
        default:
            QHash<int, QByteArray> roles = roleNames();
            if (roles.contains(role) && m_rolesCallBack.isCallable()) {
                QJSEngine *engine = qjsEngine(this);
                // WARNING! Close your eyes you don't want to see this!!!!
                value = const_cast<QJSValue *>(&m_rolesCallBack)->call(
                            QJSValueList()
                            << engine->toScriptValue<QVariant>(QString::fromLatin1(roles.value(role)))
                            << QJSValue(key)
                            << engine->toScriptValue<QVariant>(m_db->get(key))
                            ).toVariant();
            }
        }
    }
    return value;
}

int MazDBListModel::rowCount(const QModelIndex &parent) const
{
    return count();
}

MazDBListModel::FilterPolicy MazDBListModel::filterPolicy() const
{
    return m_filterPolicy;
}

QStringList MazDBListModel::customRoles() const
{
    return m_customRoles;
}

QStringList MazDBListModel::excludeKeys() const
{
    return m_excludeKeys;
}

QJSValue MazDBListModel::rolesCallBack() const
{
    return m_rolesCallBack;
}

void MazDBListModel::setSource(const QString &src)
{
    if (source() != src) {
        m_keys.clear();
        m_db->setFilename(src);
        load();
    }
}

void MazDBListModel::setFilter(const QString &filter)
{
    m_filter = filter;
    emit filterChanged();
    reload();
}

void MazDBListModel::close()
{
    m_db->close();
}

void MazDBListModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
    m_keys.clear();
    endRemoveRows();
}

void MazDBListModel::handleKeyValueChange(const QString &key, const QVariant &value)
{
    Q_UNUSED(value);
    if (!m_keys.contains(key)) {
        if (!insertAllowed(key)) {
            return;
        }
        const int rows = count();
        beginInsertRows(QModelIndex(), rows, rows);
        m_keys.append(key);
        endInsertRows();
        emit countChanged();
    } else {
        emitDataChanged(key);
    }
}

void MazDBListModel::handleKeyValueRemoved(const QString &key)
{
    if (!m_keys.contains(key)) {
        return;
    }

    const int index = m_keys.indexOf(key);
    beginRemoveRows(QModelIndex(), index, index);
    m_keys.removeAt(index);
    endRemoveRows();
    emit countChanged();
}

void MazDBListModel::load()
{
    QStringList keys;
    if (m_range.hasRange()) {
        MazCallBack func = [&](QString key, QVariant value) {
            Q_UNUSED(value);
            if (insertAllowed(key)) {
                keys << key;
            }
            return true;
        };
        m_db->rangeStream(func, m_range.start(), m_range.end());
    } else {
        MazKeyCallBack func = [&](QString key) {
            if (insertAllowed(key)) {
                keys << key;
            }
            return true;
        };
        m_db->keyStream(func, m_filter);
    }

    const int rows = count();
    beginInsertRows(QModelIndex(), rows, rows + m_keys.count() - 1);
    m_keys.append(keys);
    endInsertRows();
    emit countChanged();
}

void MazDBListModel::reload()
{
    clear();
    load();
}

void MazDBListModel::setFilterPolicy(MazDBListModel::FilterPolicy filterPolicy)
{
    if (m_filterPolicy == filterPolicy)
        return;

    m_filterPolicy = filterPolicy;
    emit filterPolicyChanged(filterPolicy);
}

void MazDBListModel::setCustomRoles(QStringList customRoles)
{
    if (m_customRoles == customRoles)
        return;

    m_customRoles = customRoles;
    emit customRolesChanged(customRoles);
}

void MazDBListModel::setRolesCallBack(QJSValue rolesCallBack)
{
    m_rolesCallBack = rolesCallBack;
    emit rolesCallBackChanged(rolesCallBack);
}

void MazDBListModel::setExcludeKeys(QStringList keys)
{
    if (m_excludeKeys != keys) {
        m_excludeKeys = keys;
        emit excludeKeysChanged();
        reload();
    }
}

void MazDBListModel::emitDataChanged(const QString &key)
{
    QModelIndex idx = index(m_keys.indexOf(key));
    if (idx.isValid()) {
        emit dataChanged(idx, idx);
    }
}

bool MazDBListModel::insertAllowed(const QString &key)
{
    if (m_excludeKeys.contains(key)) {
        return false;
    }
    if (m_filter.isEmpty()) {
        return true;
    }
    switch(m_filterPolicy) {
    case ExactMatch:
        if (m_filter == key) {
            return true;
        }
        break;
    case Contains:
        if (key.contains(m_filter)) {
            return true;
        }
        break;
    };
    return false;
}

