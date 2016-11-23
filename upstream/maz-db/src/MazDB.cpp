#include "MazDB.h"
#include <leveldb/db.h>
#include <leveldb/comparator.h>
#include <QFileInfo>
#include <QDir>
#include <qqmlinfo.h>
#include <QJsonDocument>
#include <QQmlEngine>
#include "MazDBBatch.h"

static QHash<QString, QWeakPointer<leveldb::DB> > s_levelInstances;
static QMultiHash<QString, MazDB*> s_mazInstances;

MazDB::MazDB(QObject *parent) : QObject(parent),
    m_leveldb(Q_NULLPTR), m_opened(false), m_status(Status::Undefined), m_batch(Q_NULLPTR)
{
}

MazDB::MazDB(const QString &filename, QObject *parent) : MazDB(parent)
{
    setFilename(filename);
}

MazDB::~MazDB()
{
    close();
}

QString MazDB::filename() const
{
    return m_filename;
}

bool MazDB::opened() const
{
    return m_opened;
}

MazDB::Status MazDB::open()
{
    // First check it's not already opened
    if (m_opened) {
        setStatus(Status::IOError);
        setErrorString(QStringLiteral("Database already opened"));
        return m_status;
    }

    // Next we have to check for any already existing leveldb instances for this filename
    if (s_levelInstances.contains(m_filename) && !s_levelInstances[m_filename].isNull()) {
        m_leveldb = s_levelInstances[m_filename].toStrongRef();
        m_opened = true;
        emit openedChanged();
        setStatus(Status::Ok);
        setErrorString(QString());
    } else {
        // So this is the first time we have seen this location
        // first check the directory exists
        QFileInfo db(m_filename);
        if (!db.dir().exists()) {
            db.dir().mkpath(db.dir().absolutePath());
        }

        // now lets fire up a leveldb instance
        leveldb::DB *ldb;
        leveldb::Options options;
        options.create_if_missing = m_options.createIfMissing();
        options.error_if_exists = m_options.errorIfExists();
        options.compression = static_cast<leveldb::CompressionType>(m_options.compressionType());
        options.paranoid_checks = m_options.paranoidChecks();

        auto status = leveldb::DB::Open(options, m_filename.toStdString(), &ldb);
        if (status.ok()) {
            m_leveldb.reset(ldb);
            s_levelInstances.insert(m_filename, m_leveldb.toWeakRef());
        }
        m_opened = status.ok();
        emit openedChanged();
        setErrorString(QString::fromStdString(status.ToString()));
        Status code = parseStatusCode(status);
        setStatus(code);

    }
    if (m_opened) {
        // add this to the list of current MazDB instances.
        s_mazInstances.insertMulti(m_filename, this);
    }
    return m_status;
}

void MazDB::close()
{
    delete m_batch;
    m_batch = 0;

    m_leveldb.clear();
    QWeakPointer<leveldb::DB> pointer = s_levelInstances[m_filename];
    if (pointer.isNull())
        s_levelInstances.remove(m_filename);

    for (auto key : s_mazInstances.keys()){
        s_mazInstances.remove(key, this);
    }
    setStatus(Status::Undefined);
    m_opened = false;
    emit openedChanged();
    setErrorString(QString());
}

MazDB::Status MazDB::status() const
{
    return m_status;
}

QString MazDB::errorString() const
{
    return m_errorString;
}

MazOptions *MazDB::options()
{
    return &m_options;
}

MazDBBatch *MazDB::batch()
{
    if (m_batch) {
        delete m_batch;
    }
    m_batch = new MazDBBatch(m_leveldb.toWeakRef(),this);
    connect(m_batch, &MazDBBatch::batchWritten, this, &MazDB::onBatchWritten);
    return m_batch;
}

bool MazDB::del(const QString &key)
{
    leveldb::WriteOptions options;
    leveldb::Status status = m_leveldb.data()->Delete(options, leveldb::Slice(key.toStdString()));
    if (status.ok()) {
        emitKeyValueRemoved(key);
    }
    return status.ok();
}

QVariant MazDB::get(const QString &key, QVariant defaultValue)
{
    leveldb::ReadOptions options;
    std::string value = "";
    if (m_opened && !m_leveldb.isNull()){
        leveldb::Status status = m_leveldb.data()->Get(options,
                                                leveldb::Slice(key.toStdString()),
                                                &value);
        if (status.ok())
            return MazUtils::jsonToVariant(QString::fromStdString(value));
    }
    return defaultValue;
}

bool MazDB::put(const QString &key, const QVariant &value)
{
    QVariant oldValue = get(key);
    //avoid inifine loops
    if(oldValue == value)
        return true;
    leveldb::WriteOptions options;
    QString json = MazUtils::variantToJson(value);
    if (m_opened && m_leveldb){
        leveldb::Status status = m_leveldb->Put(options,
                                                leveldb::Slice(key.toStdString()),
                                                leveldb::Slice(json.toStdString()));
        if(status.ok()){
            emitPropertyChanged(key, value);
        }
        return status.ok();
    }
    return false;
}

bool MazDB::putSync(const QString &key, const QVariant &value)
{
    QVariant oldValue = get(key);
    //avoid inifine loops
    if(oldValue == value)
        return true;
    leveldb::WriteOptions options;
    QString json = MazUtils::variantToJson(value);
    options.sync = true;
    if (m_opened && m_leveldb){
        leveldb::Status status = m_leveldb->Put(options,
                                                leveldb::Slice(key.toStdString()),
                                                leveldb::Slice(json.toStdString()));
        if(status.ok()){
            emitPropertyChanged(key, value);
        }
        return status.ok();
    }
    return false;
}

bool MazDB::destroyDB(const QString &filename)
{
    if(m_filename == filename){
        setFilename(QString());
    }
    leveldb::Options options;
    leveldb::Status status = leveldb::DestroyDB(filename.toStdString(), options);
    setStatus(parseStatusCode(status));
    setErrorString(QString::fromStdString(status.ToString()));
    return m_status == Status::Ok;
}

bool MazDB::repairDB(const QString &filename)
{
    leveldb::Options options;
    leveldb::Status status = leveldb::RepairDB(filename.toStdString(), options);
    return status.ok();
}

bool MazDB::readStream(MazCallBack callback, const QString startKey, const int length)
{
    if (!opened() || length == 0)
        return false;

    int llength = length;
    leveldb::ReadOptions options;
    leveldb::Iterator *it = m_leveldb.data()->NewIterator(options);

    if (!it)
        return false;

    QMutexLocker l(&m_mutex);
    if (!startKey.isEmpty()) {
        it->Seek(leveldb::Slice(startKey.toStdString()));
    } else {
        it->SeekToFirst();
    }

    for ( ;it->Valid(); it->Next()){

        QString key = QString::fromStdString(it->key().ToString());
        QVariant value = MazUtils::jsonToVariant(QString::fromStdString(it->value().ToString()));

        llength--;
        bool shouldContinue = callback(key, value);

        if (!shouldContinue || (length != -1 && llength <= 0))
            break;
    }
    delete it;
    return true;
}

bool MazDB::readStream(QJSValue callback, QString startKey, int length)
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
    return readStream(func, startKey, length);
}

bool MazDB::keyStream(MazKeyCallBack callback, const QString startKey, const int length)
{
    if (!opened() || length == 0)
        return false;

    int llength = length;
    leveldb::ReadOptions options;
    leveldb::Iterator *it = m_leveldb.data()->NewIterator(options);

    if (!it)
        return false;

    QMutexLocker l(&m_mutex);
    if (!startKey.isEmpty()) {
        it->Seek(leveldb::Slice(startKey.toStdString()));
    } else {
        it->SeekToFirst();
    }

    for ( ;it->Valid(); it->Next()){

        QString key = QString::fromStdString(it->key().ToString());

        llength--;
        bool shouldContinue = callback(key);

        if (!shouldContinue || (length != -1 && llength <= 0))
            break;
    }
    delete it;
    return true;
}

bool MazDB::keyStream(QJSValue callback, QString startKey, int length)
{
    if (!callback.isCallable())
        return false;

    MazKeyCallBack func = [this, &callback](QString key) {
        QJSValueList list;
        list << QJSValue(key);
        bool result = callback.call(list).toBool();
        return result;
    };
    return keyStream(func, startKey, length);
}

bool MazDB::valueStream(MazValueCallBack callback, const QString startKey, const int length)
{
    if (!opened() || length == 0)
        return false;

    int llength = length;
    leveldb::ReadOptions options;
    leveldb::Iterator *it = m_leveldb.data()->NewIterator(options);

    if (!it)
        return false;

    QMutexLocker l(&m_mutex);
    if (!startKey.isEmpty()) {
        it->Seek(leveldb::Slice(startKey.toStdString()));
    } else {
        it->SeekToFirst();
    }

    for ( ;it->Valid(); it->Next()){

        QString key = QString::fromStdString(it->key().ToString());
        QVariant value = MazUtils::jsonToVariant(QString::fromStdString(it->value().ToString()));

        llength--;
        bool shouldContinue = callback(value);

        if (!shouldContinue || (length != -1 && llength <= 0))
            break;
    }
    delete it;
    return true;
}

bool MazDB::valueStream(QJSValue callback, QString startKey, int length)
{
    if (!callback.isCallable())
        return false;

    MazValueCallBack func = [this, &callback](QVariant value) {
        QJSEngine *engine = qjsEngine(this);
        QJSValueList list;
        list << engine->toScriptValue<QVariant>(value);
        bool result = callback.call(list).toBool();
        return result;
    };
    return valueStream(func, startKey, length);
}

bool MazDB::rangeStream(MazCallBack callback, const QString startKey, const QString endKey)
{
    if (!opened())
        return false;

    if (startKey.isEmpty() || endKey.isEmpty() || endKey < startKey) {
        return false;
    }

    leveldb::Options options;
    leveldb::Iterator *it = m_leveldb.data()->NewIterator(leveldb::ReadOptions());

    if (!it)
        return false;


    QMutexLocker l(&m_mutex);

    leveldb::Slice startSlice(startKey.toStdString());
    leveldb::Slice endSlice(endKey.toStdString());

    for (it->Seek(startSlice);
         it->Valid() && options.comparator->Compare(it->key(), endSlice) <= 0;
         it->Next()) {
        QString key = QString::fromStdString(it->key().ToString());
        QVariant value = MazUtils::jsonToVariant(QString::fromStdString(it->value().ToString()));

        bool shouldContinue = callback(key, value);
        if (!shouldContinue) {
            break;
        }
    }

    delete it;
    return true;
}

bool MazDB::rangeStream(QJSValue callback, QString startKey, const QString endKey)
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
    return rangeStream(func, startKey, endKey);
}

void MazDB::setFilename(const QString &name)
{
    if (name != m_filename) {
        if (opened()) {
            close();
        }
        m_filename = name;
        emit filenameChanged();
        if (!m_filename.isEmpty()) {
            open();
        }
    }
}

void MazDB::onBatchWritten(QSet<QString> keys)
{
    for (auto key : keys){
        emitPropertyChanged(key, get(key, QVariant()));
    }
}

void MazDB::setStatus(const MazDB::Status status)
{
    if (status != m_status) {
        m_status = status;
        emit statusChanged(m_status);
    }
}

void MazDB::setErrorString(const QString &errorString)
{
    if (errorString != m_errorString) {
        m_errorString = errorString;
        emit errorStringChanged(m_errorString);
    }
}

MazDB::Status MazDB::parseStatusCode(leveldb::Status &status)
{
    if (status.ok())
        return Status::Ok;
    if (status.IsCorruption())
        return Status::Corruption;
    if (status.IsIOError())
        return Status::IOError;
    if (status.IsNotFound())
        return Status::NotFound;
    return Status::Undefined;
}

void MazDB::emitPropertyChanged(const QString &key, const QVariant &value)
{
    QMultiHash<QString, MazDB*>::iterator i = s_mazInstances.find(m_filename);
    while (i != s_mazInstances.end() && i.key() == m_filename) {
       emit i.value()->keyValueChanged(key, value);
        ++i;
    }
}

void MazDB::emitKeyValueRemoved(const QString &key)
{
    QMultiHash<QString, MazDB*>::iterator i = s_mazInstances.find(m_filename);
    while (i != s_mazInstances.end() && i.key() == m_filename) {
       emit i.value()->keyValueRemoved(key);
        ++i;
    }
}

void MazDB::classBegin()
{

}

void MazDB::componentComplete()
{

}

