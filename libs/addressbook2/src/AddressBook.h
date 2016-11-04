#ifndef ADDRESSBOOK_H
#define ADDRESSBOOK_H

#include <QObject>
#include <QJsonObject>
#include <QmlObjectListModel.h>
#include "Collection.h"

class AddressBook : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString path READ path CONSTANT)
    Q_PROPERTY(QString settingsUrl READ settings CONSTANT)
    Q_PROPERTY(bool canRead READ canRead CONSTANT)
    Q_PROPERTY(bool canWrite READ canWrite CONSTANT)
    Q_PROPERTY(bool canDelete READ canDelete CONSTANT)
    Q_PROPERTY(bool canSync READ canSync CONSTANT)
    Q_PROPERTY(bool multipleCollections READ multipleCollections CONSTANT)
    Q_PROPERTY(QString displayName READ displayName NOTIFY configChanged)
    Q_PROPERTY(QObject *collections READ collections NOTIFY collectionsChanged)
public:
    explicit AddressBook(QObject *parent = 0);
    explicit AddressBook(QObject *parent, const QVariantMap &config);

    QString name() const;
    QString displayName() const;
    QString path() const;
    QString settings() const;
    bool canRead() const;
    bool canWrite() const;
    bool canDelete() const;
    bool canSync() const;
    bool multipleCollections() const;
    void setConfig(const QVariantMap &config);
    QObject *collections() const;

    Collection *addCollection(const QString &collection);

signals:
    void configChanged();
    void collectionsChanged();


private slots:
    void handleKeyValueChanged(const QString &key, const QVariant &value);
    void handleKeyRemoved(const QString &key);

private:
    QQmlObjectListModel<Collection> *m_collections;
    QVariantMap m_config;
    QVariantMap m_options;
};


#endif // ADDRESSBOOK_H
