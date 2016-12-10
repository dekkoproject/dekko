#ifndef ITEMREGISTRY_H
#define ITEMREGISTRY_H

#include <QObject>
#include <QPointer>
#include <QQuickItem>
#include <QQmlListProperty>
#include "PluginIncubator.h"

class ItemRegistry : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QString location READ location WRITE setLocation NOTIFY locationChanged)
    Q_PROPERTY(QQmlListProperty<QQuickItem> defaultItems READ defaultItems)
    Q_PROPERTY(LoadMode loadMode READ loadMode WRITE setLoadMode NOTIFY loadModeChanged)
    Q_PROPERTY(QString pluginId MEMBER m_pluginId)
    // Should plugins be loaded asynchonously. Default items are always created synchronously
    Q_PROPERTY(bool asynchronous READ asynchronous WRITE setAsynchronous NOTIFY asyncChanged)
    Q_ENUMS(LoadMode)

public:
    explicit ItemRegistry(QObject *parent = 0);

    enum LoadMode {
        DefaultOnly,
        LoadFirstEnabled,
        LoadLastEnabled,
        LoadAll,
        LoadById // will fallback to default if a plugin with id doesn't exist.
    };

    QQuickItem *target() const;
    QString location() const;
    QQmlListProperty<QQuickItem> defaultItems();

    LoadMode loadMode() const;
    bool asynchronous() const;

    Q_INVOKABLE QString findFirstEnabled(const QString &location);

signals:
    void targetChanged(QQuickItem *target);
    void locationChanged(QString location);
    void loadModeChanged(LoadMode loadMode);

    void asyncChanged(bool asynchronous);

public slots:
    void setTarget(QQuickItem *target);
    void setLocation(QString location);
    void setLoadMode(LoadMode loadMode);
    void setAsynchronous(bool asynchronous);

private slots:
    void loadIfPossible();
    void reparentItemToTarget(QQuickItem *item);
    void reparentItemsToTarget(QList<QQuickItem *> items);
    QQuickItem *createItemFromUrl(const QString &itemUrl);
    void createItemAsync(const QString &itemUrl);
    void asyncItemReady();
    void handleIncubatorError();

private:
    LoadMode m_loadMode;
    QPointer<QQuickItem> m_target;
    QString m_location;
    QList<QQuickItem *> m_defaultItems;
    QString m_pluginId;
    bool m_asynchronous;
    IncubatorList m_incubators;
};

#endif // ITEMREGISTRY_H
