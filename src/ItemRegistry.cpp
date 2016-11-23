#include "ItemRegistry.h"
#include "PluginRegistry.h"

ItemRegistry::ItemRegistry(QObject *parent) : QObject(parent),
    m_loadMode(LoadAll)
{
}

QQuickItem* ItemRegistry::target() const
{
    return m_target;
}

QString ItemRegistry::location() const
{
    return m_location;
}

QQmlListProperty<QQuickItem> ItemRegistry::defaultItems()
{
    return QQmlListProperty<QQuickItem>(this, m_defaultItems);
}

ItemRegistry::LoadMode ItemRegistry::loadMode() const
{
    return m_loadMode;
}

void ItemRegistry::setTarget(QQuickItem *target)
{
    if (m_target == target)
        return;

    m_target = target;
    emit targetChanged(target);
    loadIfPossible();
}

void ItemRegistry::setLocation(QString location)
{
    if (m_location == location)
        return;

    m_location = location;
    emit locationChanged(location);
    loadIfPossible();
}

void ItemRegistry::setLoadMode(ItemRegistry::LoadMode loadMode)
{
    if (m_loadMode == loadMode)
        return;

    m_loadMode = loadMode;
    emit loadModeChanged(loadMode);
}

void ItemRegistry::loadIfPossible()
{
    if (m_location.isEmpty() || m_target.isNull()) {
        return;
    }

    auto plugins = PluginRegistry::instance()->getByLocation(m_location);

    switch(m_loadMode) {
    case DefaultOnly:
    {
        qDebug() << "Loading default items only";
        reparentItemsToTarget(m_defaultItems);
        break;
    }
    case LoadFirstEnabled:
    {
        qDebug() << "Loading first enabled plugin";
        auto firstplugin = qobject_cast<DekkoPlugin *>(plugins.first());
        QQuickItem *firstItem = createItemFromUrl(firstplugin->component());
        reparentItemToTarget(firstItem);
        break;
    }
    case LoadLastEnabled:
    {
        qDebug() << "Loading last enabled plugin";
        auto lastplugin = qobject_cast<DekkoPlugin *>(plugins.last());
        QQuickItem *lastItem = createItemFromUrl(lastplugin->component());
        reparentItemToTarget(lastItem);
        break;
    }
    case LoadAll:
    {
        qDebug() << "Loading all plugins and default items";
        reparentItemsToTarget(m_defaultItems); // default items get appended first
        for (auto plugin : plugins) {
            if (auto dp = qobject_cast<DekkoPlugin *>(plugin)) {
                QQuickItem *item = createItemFromUrl(dp->component());
                reparentItemToTarget(item);
            }
        }
        break;
    }
    case LoadById:
    {
        qDebug() << "Looking for plugin with id: " << m_pluginId;
        QQuickItem *item = Q_NULLPTR;
        for (auto plugin : plugins) {
            if (plugin->pluginId() == m_pluginId) {
                if (auto dp = qobject_cast<DekkoPlugin *>(plugin)) {
                    qDebug() << "Found plugin with id: " << m_pluginId;
                    item = createItemFromUrl(dp->component());
                }
            }
        }
        if (item) {
            reparentItemToTarget(item);
        } else {
            qDebug() << "No plugin found with id: " << m_pluginId;
            qDebug() << "Falling back to default items";
            reparentItemsToTarget(m_defaultItems);
        }
        break;
    }
    }
}

void ItemRegistry::reparentItemToTarget(QQuickItem *item)
{
    if (item) {
        item->setParentItem(m_target);
        item->setVisible(true);
    } else {
        qWarning() << "Invalid item";
    }
}

void ItemRegistry::reparentItemsToTarget(QList<QQuickItem *> items)
{
    for (auto item : items) {
        reparentItemToTarget(item);
    }
}

QQuickItem *ItemRegistry::createItemFromUrl(const QString &itemUrl)
{
    if (itemUrl.isEmpty()) {
        qWarning() << "Invalid component url";
        return Q_NULLPTR;
    }
    auto engine = qmlEngine(this);
    QQmlComponent itemComponent(engine, QUrl::fromLocalFile(itemUrl));
    if (itemComponent.isError()) {
        for (auto error : itemComponent.errors()) {
            qDebug() << "Failed loading plugin with error:";
            qDebug() << error.toString();
        }
        return Q_NULLPTR;
    }
    return qobject_cast<QQuickItem *>(itemComponent.create());
}

