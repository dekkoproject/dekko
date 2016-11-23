#include "MazDBQuery.h"
#include <QDebug>
#include <QRect>
#include <QRectF>
#include <functional>
#include <QQmlEngine>

QueryBase::QueryBase(QObject *parent) : QObject(parent),
    m_matchRule(Equals)
{
}

QString QueryBase::prop() const
{
    return m_property;
}

QVariant QueryBase::value() const
{
    return m_value;
}

QueryBase::MatchTypes QueryBase::matchType() const
{
    return m_matchRule;
}

bool QueryBase::hasMatch(const QVariant &value)
{
    if (m_comparitor.isCallable()) {
        QJSEngine *engine = qjsEngine(this);
        QJSValueList list;
        list << engine->toScriptValue<QVariant>(value);
        return m_comparitor.call(list).toBool();
    }

    if (m_value.isNull()) {
        return true;
    }

    bool matches = false;
    QStringList supported;
    switch(value.type()) {
    case QVariant::Invalid:
        matches = false;
        break;
    case QVariant::Uuid:
    case QVariant::Url:
    case QVariant::String:
    case QVariant::ByteArray:
    case QVariant::LongLong:
    case QVariant::ULongLong:
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::Bool:
    case QVariant::Double:
    case QVariant::Char:
    case QVariant::KeySequence:
    case QMetaType::QDate:
    case QMetaType::QTime:
    case QMetaType::QDateTime:{
        matches = compare(value);
        break;
    }
    case QVariant::Rect: {
        supported << "x" << "y" << "width" << "height";
        if (!m_property.isEmpty() && supported.contains(m_property)) {
            QRect r = qvariant_cast<QRect>(value);
            if (m_property == "x") {
                matches = compare(r.x());
            } else if (m_property == "y") {
                matches = compare(r.y());
            } else if (m_property == "height") {
                matches = compare(r.height());
            } else if (m_property == "width") {
                matches = compare(r.width());
            }
        } else {
            matches = compare(value);
        }
        break;
    }
    case QVariant::RectF: {
        supported << "x" << "y" << "width" << "height";
        if (!m_property.isEmpty() && supported.contains(m_property)) {
            QRectF r = qvariant_cast<QRectF>(value);
            if (m_property == "x") {
                matches = compare(r.x());
            } else if (m_property == "y") {
                matches = compare(r.y());
            } else if (m_property == "height") {
                matches = compare(r.height());
            } else if (m_property == "width") {
                matches = compare(r.width());
            }
        } else {
            matches = compare(value);
        }
        break;
    }
    case QVariant::Size: {
        supported << "width" << "height";
        if (!m_property.isEmpty() && supported.contains(m_property)) {
            QSize s = qvariant_cast<QSize>(value);
            if (m_property == "height") {
                matches = compare(s.height());
            } else if (m_property == "width") {
                matches = compare(s.width());
            }
        } else {
            matches = compare(value);
        }
        break;
    }
    case QVariant::SizeF: {
        supported << "width" << "height";
        if (!m_property.isEmpty() && supported.contains(m_property)) {
            QSizeF s = qvariant_cast<QSizeF>(value);
            if (m_property == "height") {
                matches = compare(s.height());
            } else if (m_property == "width") {
                matches = compare(s.width());
            }
        } else {
            matches = compare(value);
        }
        break;
    }
    case QVariant::Point: {
        supported << "x" << "y";
        if (!m_property.isEmpty() && supported.contains(m_property)) {
            QPoint p = qvariant_cast<QPoint>(value);
            if (m_property == "x") {
                matches = compare(p.x());
            } else if (m_property == "y") {
                matches = compare(p.y());
            }
        } else {
            matches = compare(value);
        }
        break;
    }
    case QVariant::PointF: {
        supported << "x" << "y";
        if (!m_property.isEmpty() && supported.contains(m_property)) {
            QPointF p = qvariant_cast<QPointF>(value);
            if (m_property == "x") {
                matches = compare(p.x());
            } else if (m_property == "y") {
                matches = compare(p.y());
            }
        } else {
            matches = compare(value);
        }
        break;
    }
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap: {
        if (!m_property.isEmpty()) {
            QVariantMap map = value.toMap();
            if (map.contains(m_property)) {
                matches = compare(map.value(m_property));
            } else if (m_property == "*") {
                for (auto key : map.keys()) {
                    matches = compare(map.value(key));
                    if (matches) {
                        break;
                    }
                }
            } else {
                QStringList path = m_property.split('.', QString::SplitBehavior::SkipEmptyParts);
                QVariant val = value;
                for (const QString &key : path) {
                    QVariantMap m = val.toMap();
                    if (m.isEmpty() || (val = m.value(key)).isNull()) {
                        return false;
                    }
                    matches = compare(val);
                }
            }
        } else {
            qWarning() << "No property declared for QVariantMap/Hash";
        }
        break;
    }


    default: {
#ifndef QT_NO_DATASTREAM
#else
        Q_ASSERT(!"MazDB: Cannot save custom types without QDataStream support");
#endif
        break;
    }
    }
    return matches;
}

QJSValue QueryBase::comparitor() const
{
    return m_comparitor;
}

void QueryBase::setProp(QString property)
{
    if (m_property == property)
        return;

    m_property = property;
    emit queryChanged();
}

void QueryBase::setValue(QVariant value)
{
    if (m_value == value)
        return;

    m_value = value;
    emit queryChanged();
}

void QueryBase::setMatchType(QueryBase::MatchTypes matchRule)
{
    if (m_matchRule == matchRule)
        return;

    m_matchRule = matchRule;
    emit queryChanged();
}

void QueryBase::setComparitor(QJSValue comparitor)
{
    m_comparitor = comparitor;
    emit queryChanged();
}

bool QueryBase::compare(const QVariant &value)
{
    switch(m_matchRule) {
    case Equals:
        return value == m_value;
    case NotEquals:
        return value != m_value;
    case LessThan:
        return value < m_value;
    case LessThanOrEqual:
        return value <= m_value;
    case GreaterThan:
        return value > m_value;
    case GreaterThanOrEqual:
        return value >= m_value;
    default:
        return false;
    }
}

MazDBQuery::MazDBQuery(QObject *parent) : QObject(parent)
{
}

Range *MazDBQuery::range()
{
    return &m_range;
}

int MazDBQuery::limit() const
{
    return m_limit;
}

QQmlListProperty<QueryBase> MazDBQuery::query()
{
    return QQmlListProperty<QueryBase>(this, m_queries);
}

bool MazDBQuery::runQuery(MazDB *db, MazCallBack callback)
{
    QStringList keys;

    if (m_range.hasRange()) {
        MazCallBack func = [&keys](QString key, QVariant value) {
            Q_UNUSED(value);
            keys << key;
            return true;
        };
        db->rangeStream(func, m_range.start(), m_range.end());
    } else {
        MazKeyCallBack func = [&keys](QString key) {
            keys << key;
            return true;
        };
        db->keyStream(func);
    }

    if (keys.isEmpty()) {
        return false;
    }

    auto runWHEREorORqueries = [&](const QStringList &keys) -> QStringList {
        QStringList r;
        WhereQuery *whereQuery = getWhereQuery();
        OrQuery *orQuery = getOrQuery();
        if (whereQuery) {
            for (const QString &key : keys) {
                QVariant val = db->get(key);
                if (whereQuery->hasMatch(val)) {
                    r << key;
                } else if (orQuery && orQuery->hasMatch(val)) {
                    r << key;
                }
            }
        }
        return r;
    };

    auto runANDqueries = [&](const QStringList &keys) -> QStringList {
        QStringList r = keys;
        QList<AndQuery *> andQueries = getAndQueries();
        if (!andQueries.isEmpty()) {
            for (AndQuery *andQuery : andQueries) {
                QStringList tmpKeys = r;
                for (const QString &key : tmpKeys) {
                    QVariant val = db->get(key);
                    if (!andQuery->hasMatch(val)) {
                        r.removeAll(key);
                    }
                }
            }
        }
        return r;
    };

    QStringList results = runWHEREorORqueries(keys);
    if (!results.isEmpty()) {
        results = runANDqueries(results);
    }

    for (const QString &result : results) {
        callback(result, db->get(result));
    }

    return true;
}

void MazDBQuery::setLimit(int limit)
{
    if (m_limit == limit)
        return;

    m_limit = limit;
    emit limitChanged(limit);
}

WhereQuery *MazDBQuery::getWhereQuery()
{
    for (QueryBase *query : m_queries) {
        if (auto where = qobject_cast<WhereQuery *>(query)) {
            return where;
        }
    }
    qWarning() << "No WHERE clause!";
    return Q_NULLPTR;
}

OrQuery *MazDBQuery::getOrQuery()
{
    for (QueryBase *query : m_queries) {
        if (auto orQuery = qobject_cast<OrQuery*>(query)) {
            return orQuery;
        }
    }
    return Q_NULLPTR;
}

QList<AndQuery *> MazDBQuery::getAndQueries()
{
    QList<AndQuery *> r;
    for (QueryBase *query : m_queries) {
        if (auto andQuery = qobject_cast<AndQuery*>(query)) {
            r << andQuery;
        }
    }
    return r;
}

Range::Range(QObject *parent) : QObject(parent)
{
}

bool Range::hasRange() const
{
    return (!m_start.isEmpty() && !m_end.isEmpty());
}

QString Range::start() const
{
    return m_start;
}

QString Range::end() const
{
    return m_end;
}

void Range::setStart(QString start)
{
    if (m_start == start)
        return;

    m_start = start;
    emit startChanged(start);
}

void Range::setEnd(QString end)
{
    if (m_end == end)
        return;

    m_end = end;
    emit endChanged(end);
}
