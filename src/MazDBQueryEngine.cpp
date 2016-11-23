#include "MazDBQueryEngine.h"
#include <QQmlEngine>
#include <QDebug>

MazDBQueryEngine::MazDBQueryEngine(QObject *parent) : QObject(parent),
    m_db(Q_NULLPTR), m_query(Q_NULLPTR)
{
    m_db = new MazDB(this);
    connect(m_db, &MazDB::filenameChanged, this, &MazDBQueryEngine::sourceChanged);
}

MazDBQuery *MazDBQueryEngine::query() const
{
    return m_query;
}

QString MazDBQueryEngine::source() const
{
    return m_db->filename();
}

bool MazDBQueryEngine::queryStream(MazCallBack callback)
{
    return m_query->runQuery(m_db, callback);
}

bool MazDBQueryEngine::queryStream(QJSValue callback)
{
    if (!callback.isCallable())
        return false;

    MazCallBack func = [this, &callback](QString key, QVariant value) {
        QJSEngine *engine = qjsEngine(this);
        QJSValueList list;
        list << QJSValue(key);
        list << engine->toScriptValue<QVariant>(value);
        bool result = callback.call(list).toBool();
        return result;
    };
    return queryStream(func);
}

void MazDBQueryEngine::setSource(const QString &src)
{
    if (source() != src) {
        m_db->setFilename(src);
    }
}

void MazDBQueryEngine::setQuery(MazDBQuery *query)
{
    if (m_query == query)
        return;

    m_query = query;
    emit queryChanged(query);
}

