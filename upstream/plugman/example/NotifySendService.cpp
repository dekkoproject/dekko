#include "NotifySendService.h"

NotifySendService::NotifySendService(QObject *parent) : ServicePlugin(parent)
{

}

QString NotifySendService::pluginId() const
{
    return QStringLiteral("notify-send-service");
}

QString NotifySendService::location() const
{
    return QStringLiteral("Dekko::Service");
}

QString NotifySendService::i18n() const
{
    return QString();
}

QVariantMap NotifySendService::documentation() const
{
    return QVariantMap();
}

void NotifySendService::start() {
    QStringList options;
    options.append("-i");
    options.append("/usr/share/icons/dekko.png");
    options.append(QStringLiteral("Hello Service"));
    options.append(QStringLiteral("This is pretty cool"));
    m_process.start("notify-send", options);
}


QString NotifySendPlugin::name() const
{
    return QStringLiteral("notify-send-service");
}

QString NotifySendPlugin::description() const
{
    return QStringLiteral("Plugin that sends a simple notification");
}

PluginInfo *NotifySendPlugin::create(QObject *parent) const
{
    return new NotifySendService(parent);
}
