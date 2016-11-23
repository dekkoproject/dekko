#include "MazDBBatch.h"
#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include <leveldb/options.h>

MazDBBatch::MazDBBatch(QWeakPointer<leveldb::DB> db, QObject *parent) : QObject(parent),
    m_leveldb(db), m_batch(new leveldb::WriteBatch())
{

}

MazDBBatch::~MazDBBatch()
{
    if (m_batch) {
        delete m_batch;
        m_batch = Q_NULLPTR;
    }
}

MazDBBatch *MazDBBatch::del(const QString &key)
{
    m_keys.insert(key);
    QMutexLocker l(&m_mutex);
    m_batch->Delete(leveldb::Slice(key.toStdString()));
    return this;
}

MazDBBatch *MazDBBatch::put(const QString &key, const QVariant &value)
{
    QString json = MazUtils::variantToJson(value);
    m_keys.insert(key);
    QMutexLocker l(&m_mutex);
    m_batch->Put(leveldb::Slice(key.toStdString()),
                 leveldb::Slice(json.toStdString()));
    return this;
}

MazDBBatch *MazDBBatch::clear()
{
    m_keys.clear();
    QMutexLocker l(&m_mutex);
    m_batch->Clear();
    return this;
}

bool MazDBBatch::write()
{
    leveldb::WriteOptions options;
    options.sync = true;
    if(m_leveldb.isNull())
        return static_cast<int>(MazDB::Status::NotFound);
    leveldb::Status status = m_leveldb.data()->Write(options, m_batch);
    if(status.ok()){
        emit batchWritten(m_keys);
    }
    return status.ok();
}

