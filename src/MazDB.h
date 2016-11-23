#ifndef MAZDB_H
#define MAZDB_H

#include <QObject>
#include <QMutex>
#include <functional>
#include <QSharedPointer>
#include <QVariant>
#include <QString>
#include <QJSValue>
#include <QQmlParserStatus>
#include "MazOptions.h"
#include "MazUtils.h"

namespace leveldb {
class DB;
class Status;
}

class MazDBBatch;

typedef std::function<bool(QString, QVariant)> MazCallBack;
typedef std::function<bool(QString)> MazKeyCallBack;
typedef std::function<bool(QVariant)> MazValueCallBack;

class MazDB : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString source READ filename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(bool opened READ opened NOTIFY openedChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)
    Q_PROPERTY(MazOptions* options READ options CONSTANT)
    Q_ENUMS(Status)
public:
    explicit MazDB(QObject *parent = 0);
    explicit MazDB(const QString &filename, QObject *parent = 0);
    ~MazDB();

    enum Status {
        Undefined = -1,
        Ok = 0,
        NotFound = 1,
        Corruption = 2,
        NotSupported = 3,
        InvalidArgument = 4,
        IOError = 5
    };

    QString filename() const;
    bool opened() const;
    Status open();
    Q_INVOKABLE void close();
    Status status() const;
    QString errorString() const;
    MazOptions *options();

    Q_INVOKABLE MazDBBatch *batch();
    Q_INVOKABLE bool del(const QString &key);
    Q_INVOKABLE QVariant get(const QString &key, QVariant defaultValue = QVariant());
    Q_INVOKABLE bool put(const QString &key, const QVariant &value);
    Q_INVOKABLE bool putSync(const QString &key, const QVariant &value);
    Q_INVOKABLE bool destroyDB(const QString &filename);
    Q_INVOKABLE bool repairDB(const QString &filename);

    // Streams API - matches those of levelup plus we add a rangeStream
    bool readStream(MazCallBack callback, const QString startKey = QString(), const int length = -1);
    Q_INVOKABLE bool readStream(QJSValue callback, QString startKey = QString(), int length = -1);
    bool keyStream(MazKeyCallBack callback, const QString startKey = QString(), const int length = -1);
    Q_INVOKABLE bool keyStream(QJSValue callback, QString startKey = QString(), int length = -1);
    bool valueStream(MazValueCallBack callback, const QString startKey = QString(), const int length = -1);
    Q_INVOKABLE bool valueStream(QJSValue callback, QString startKey = QString(), int length = -1);
    bool rangeStream(MazCallBack callback, const QString startKey, const QString endKey);
    Q_INVOKABLE bool rangeStream(QJSValue callback, QString startKey, const QString endKey);

signals:
    void filenameChanged();
    void openedChanged();
    void statusChanged(Status status);
    void errorStringChanged(QString errorString);
    void keyValueChanged(QString key, QVariant value);
    void keyValueRemoved(QString key);

public slots:
    void setFilename(const QString &name);

protected:
    QSharedPointer<leveldb::DB> m_leveldb;
    virtual void classBegin() override;
    virtual void componentComplete() override;

private slots:
    void onBatchWritten(QSet<QString> keys);
private:
    void setStatus(const Status status);
    void setErrorString(const QString &errorString);
    Status parseStatusCode(leveldb::Status &status);
    void emitPropertyChanged(const QString &key, const QVariant &value);
    void emitKeyValueRemoved(const QString &key);

    QString m_filename;
    bool m_opened;
    Status m_status;
    QString m_errorString;
    MazOptions m_options;
    MazDBBatch *m_batch;
    QMutex m_mutex;
};

#endif // MAZDB_H
