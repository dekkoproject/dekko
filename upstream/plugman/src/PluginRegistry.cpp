#include "PluginRegistry.h"
#include <QPointer>
#include <QDebug>
#include <QStandardPaths>

PluginRegistry::PluginRegistry(QObject *parent) : QObject(parent),
    m_loader(new PluginLoader),
    m_regConf(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
              QStringLiteral("/PluginRegistry.conf"), QSettings::IniFormat)
{
    m_regConf.setValue("version", "0.1");
}

PluginRegistry::~PluginRegistry()
{
    qDeleteAll(m_registry);
    m_registry.clear();
}

static QPointer<PluginRegistry> s_registry;
PluginRegistry *PluginRegistry::instance()
{
    if (s_registry.isNull()) {
        qDebug() << "Creating instance";
        s_registry = new PluginRegistry();
    }
    return s_registry;
}

Registry PluginRegistry::registry() const
{
    return m_registry;
}

bool PluginRegistry::supportsLocation(const QString &location)
{
    return m_locations.contains(location);
}

QStringList PluginRegistry::pluginLocations() const
{
    return m_locations;
}

void PluginRegistry::setPluginLocations(const QStringList &locations)
{
    m_locations = locations;
}

void PluginRegistry::loadPlugins(const QStringList &paths)
{
    qDeleteAll(m_registry);
    m_registry.clear();
    if (!m_loader.isNull()) {
        m_loader->setPluginPaths(paths);
        m_loader->loadPlugins(m_registry);
        updateRegistryConf();
    }
}

QList<PluginInfo *> PluginRegistry::getAllPlugins(const bool enabledOnly)
{
    if (enabledOnly) {
        QList<PluginInfo *> plugins;
        for (auto plugin : m_registry.values()) {
            m_regConf.beginGroup(plugin->pluginId());
            const bool enabled = m_regConf.value(QStringLiteral("enabled"), true).toBool();
            m_regConf.endGroup();
            if (enabled) {
                plugins << plugin;
            }
        }
        return plugins;
    }
    return m_registry.values();
}

QList<PluginInfo *> PluginRegistry::getByLocation(const QString &key, const bool enabledOnly)
{
    if (!m_locations.contains(key)) {
        qWarning() << "Location not supported: " << key;
        return QList<PluginInfo *>();
    }
    if (enabledOnly) {
        qDebug() << "Getting all enabled plugins";
        QList<PluginInfo *> plugins;
        for (auto plugin : m_registry.values(key)) {
            m_regConf.beginGroup(plugin->pluginId());
            bool enabled = m_regConf.value(QStringLiteral("enabled"), true).toBool();
            m_regConf.endGroup();
            if (enabled) {
                qDebug() << "Plugin enabled: " << plugin->pluginId();
                plugins << plugin;
            }
        }
        return plugins;
    }
    return m_registry.values(key);
}

void PluginRegistry::enablePlugin(const QString &id)
{
    m_regConf.beginGroup(id);
    m_regConf.setValue("enabled", true);
    m_regConf.endGroup();
}

void PluginRegistry::disablePlugin(const QString &id)
{
    m_regConf.beginGroup(id);
    m_regConf.setValue("enabled", false);
    m_regConf.endGroup();
}

void PluginRegistry::updateRegistryConf()
{
    const QStringList keys = m_regConf.childGroups();
    // we enable unknown plugins by default
    for (auto plugin : m_registry.values()) {
        if (!keys.contains(plugin->pluginId())) {
            qDebug() << plugin->pluginId() << "Not in" << keys;
            enablePlugin(plugin->pluginId());
        }
    }
    m_regConf.sync();
}

