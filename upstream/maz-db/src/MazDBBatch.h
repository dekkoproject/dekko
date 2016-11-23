#ifndef MAZDBBATCH_H
#define MAZDBBATCH_H

#include <QObject>
#include <QMutex>
#include <QSet>
#include <QScopedPointer>
#include "MazDB.h"
#include "MazUtils.h"

namespace leveldb {
class DB;
class WriteBatch;
}

class MazDBBatch : public QObject
{
    Q_OBJECT
public:
    explicit MazDBBatch(QWeakPointer<leveldb::DB> db, QObject *parent = 0);
    ~MazDBBatch();

    Q_INVOKABLE MazDBBatch *del(const QString &key);
    Q_INVOKABLE MazDBBatch *put(const QString &key, const QVariant &value);
    Q_INVOKABLE MazDBBatch *clear();
    Q_INVOKABLE bool write();

signals:
    void batchWritten(QSet<QString> keys);

private:
    Q_DISABLE_COPY(MazDBBatch)
    QSharedPointer<leveldb::DB> m_leveldb;
    leveldb::WriteBatch *m_batch;
    QSet<QString> m_keys;
    QMutex m_mutex;
};

#endif // MAZDBBATCH_H
