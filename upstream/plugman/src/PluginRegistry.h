#ifndef PLUGINREGISTRY_H
#define PLUGINREGISTRY_H

#include <QObject>
#include <QMultiMap>
#include <QSharedPointer>
#include <QSettings>
#include "PluginLoader.h"

// key is plugin location i.e "Composer::ContextAction"
// we use a multimap as we may have multiple plugins for the same location
typedef QMultiMap<QString, PluginInfo *> Registry;

class PluginRegistry : public QObject
{
    Q_OBJECT
public:
    explicit PluginRegistry(QObject *parent = 0);
    ~PluginRegistry();

    static PluginRegistry *instance();

    Registry registry() const;
    bool supportsLocation(const QString &location);
    QStringList pluginLocations() const;
    void setPluginLocations(const QStringList &locations);
    void loadPlugins(const QStringList &paths = QStringList());

    QList<PluginInfo *> getAllPlugins(const bool enabledOnly = true);
    QList<PluginInfo *> getByLocation(const QString &key, const bool enabledOnly = true);

signals:
    void pluginsLoaded();

public slots:
    void enablePlugin(const QString &id);
    void disablePlugin(const QString &id);

protected:
    void updateRegistryConf();

private:
    Registry m_registry;
    QScopedPointer<PluginLoader> m_loader;
    QStringList m_locations;
    QSettings m_regConf;
};

#endif // PLUGINREGISTRY_H
