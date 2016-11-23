#include "ServiceRegistry.h"
#include "PluginRegistry.h"

ServiceRegistry::ServiceRegistry(QObject *parent) : QObject(parent)
{
    connect(this, &ServiceRegistry::serviceKeyChanged, this, &ServiceRegistry::loadServices);
}

ServiceRegistry::~ServiceRegistry()
{
    clear();
}

void ServiceRegistry::setServiceKey(const QString &key)
{
    m_key = key;
    emit serviceKeyChanged();
}

void ServiceRegistry::startServices()
{
    for (auto service : m_services) {
        service->start();
    }
}

void ServiceRegistry::killServices()
{
    for (auto service : m_services) {
        service->stop();
    }
}

void ServiceRegistry::clear()
{
    killServices();
    qDeleteAll(m_services);
    m_services.clear();
}

void ServiceRegistry::loadServices()
{
    auto plugins = PluginRegistry::instance()->getByLocation(m_key);
    for (auto plugin : plugins) {
        qDebug() << "Found Service plugin " << plugin->pluginId();
        if (auto servicePlugin = qobject_cast<ServicePlugin *>(plugin)) {
            qDebug() << "We have a service plugin!!!" << servicePlugin->location();
            m_services.append(servicePlugin);
        }
    }
}
