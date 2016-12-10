#include "ItemRegistry.h"
#include "PluginRegistry.h"

ItemRegistry::ItemRegistry(QObject *parent) : QObject(parent),
    m_loadMode(LoadAll), m_asynchronous(true)
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

bool ItemRegistry::asynchronous() const
{
    return m_asynchronous;
}

QString ItemRegistry::findFirstEnabled(const QString &location)
{
    if (location.isEmpty()) {
        return QString();
    }
    auto plugins = PluginRegistry::instance()->getByLocation(m_location);
    if (plugins.isEmpty()) {
        return QString();
    }
    qDebug() << "Found first enabled plugin";
    auto firstplugin = qobject_cast<DekkoPlugin *>(plugins.first());
    return firstplugin->component();
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

void ItemRegistry::setAsynchronous(bool asynchronous)
{
    if (m_asynchronous == asynchronous)
        return;

    m_asynchronous = asynchronous;
    emit asyncChanged(asynchronous);
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
        if (plugins.isEmpty()) {
            setLoadMode(DefaultOnly);
            loadIfPossible();
            return;
        }
        qDebug() << "Loading first enabled plugin";
        auto firstplugin = qobject_cast<DekkoPlugin *>(plugins.first());
        if (m_asynchronous) {
            createItemAsync(firstplugin->component());
        } else {
            QQuickItem *firstItem = createItemFromUrl(firstplugin->component());
            reparentItemToTarget(firstItem);
        }
        break;
    }
    case LoadLastEnabled:
    {
        if (plugins.isEmpty()) {
            setLoadMode(DefaultOnly);
            loadIfPossible();
            return;
        }
        qDebug() << "Loading last enabled plugin";
        auto lastplugin = qobject_cast<DekkoPlugin *>(plugins.last());
        if (m_asynchronous) {
            createItemAsync(lastplugin->component());
        } else {
            QQuickItem *lastItem = createItemFromUrl(lastplugin->component());
            reparentItemToTarget(lastItem);
        }
        break;
    }
    case LoadAll:
    {
        qDebug() << "Loading all plugins and default items";
        reparentItemsToTarget(m_defaultItems); // default items get appended first
        for (auto plugin : plugins) {
            if (auto dp = qobject_cast<DekkoPlugin *>(plugin)) {
                if (m_asynchronous) {
                    createItemAsync(dp->component());
                } else {
                    QQuickItem *item = createItemFromUrl(dp->component());
                    reparentItemToTarget(item);
                }
            }
        }
        break;
    }
    case LoadById:
    {
        if (plugins.isEmpty()) {
            setLoadMode(DefaultOnly);
            loadIfPossible();
            return;
        }
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
    return qobject_cast<QQuickItem *>(itemComponent.create(engine->contextForObject(this)));
}

void ItemRegistry::createItemAsync(const QString &itemUrl)
{
    if (itemUrl.isEmpty()) {
        qWarning() << "Invalid component url";
        return;
    }
    PluginIncubator *incubator = new PluginIncubator(this);
    connect(incubator, &PluginIncubator::objectReady, this, &ItemRegistry::asyncItemReady);
    connect(incubator, &PluginIncubator::error, this, &ItemRegistry::handleIncubatorError);
    incubator->setSourceUrl(qmlEngine(this), QUrl::fromLocalFile(itemUrl));
    m_incubators << incubator;
}

void ItemRegistry::asyncItemReady()
{
    PluginIncubator *incubator = qobject_cast<PluginIncubator *>(sender());
    if (incubator->status() == QQmlIncubator::Ready) {
        QObject *itemObject = incubator->object();
        QQuickItem *item = qobject_cast<QQuickItem *>(itemObject);
        if (item) {
            reparentItemToTarget(item);
        } else {
            qWarning() << "Failed casting plugin to qquickitem";
            incubator->deleteLater();
        }
    } else {
        incubator->deleteLater();
    }
}

void ItemRegistry::handleIncubatorError()
{

}

