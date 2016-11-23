#ifndef MAZDBSETTINGS_H
#define MAZDBSETTINGS_H

#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QHash>
#include <QQmlParserStatus>

class MazDB;

typedef QList<QMetaObject::Connection> ConnectionList;

class MazDBSettings : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    // Emulates the category property from qt.labs.Settings but internally
    // it creates a new leveldb for the given category.
    Q_PROPERTY(QString category MEMBER m_category)
public:
    explicit MazDBSettings(QObject *parent = 0);

    QStringList allKeys();
    bool contains(const QString &key);
    Q_INVOKABLE QString fileName() const;
    void remove(const QString &key);
    Q_INVOKABLE void setValue(const QString &key, const QVariant &value);
    Q_INVOKABLE QVariant value(const QString &key, const QVariant &defaultValue = QVariant());
    Q_INVOKABLE void close();

protected:
    virtual void classBegin() override;
    virtual void componentComplete() override;

private slots:
    void handlePropertyChanged();
    void handleKeyValueChanged(const QString &key, const QVariant &value);
    void init();
private:
    Q_DISABLE_COPY(MazDBSettings)

    MazDB *m_db;
    ConnectionList m_connections;
    QHash<int, QMetaProperty> m_propertyHash;
    QString m_category;
};

#endif // MAZDBSETTINGS_H
