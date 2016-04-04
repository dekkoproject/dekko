#include "SettingsPlugin.h"
#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include <NavigationSettings.h>

void SettingsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Dekko.Settings"));
    // @uri Dekko.Settings
    qmlRegisterType<NavigationSettings>(uri, 1, 0, "NavigationSettings");
}

void SettingsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
