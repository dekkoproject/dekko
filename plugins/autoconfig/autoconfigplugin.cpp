#include "autoconfigplugin.h"
#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include "autodiscover.h"
#include "emailvalidator.h"

void AutoConfigPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Dekko.AutoConfig"));
    // @uri Dekko.AutoConfig
    qmlRegisterType<AutoDiscover>(uri, 1, 0, "AutoDiscover");
    qmlRegisterUncreatableType<ServerConfiguration>(uri, 1, 0, "ServerConfig", QLatin1String("ServerConfiguration cannot be instantiated from qml"));
    // export this as a singleton just incase it is handy to use for validation
    // in the qml layer
    qmlRegisterSingletonType<EmailValidator>(uri, 1, 0, "EmailValidator", EmailValidator::factory);
}

void AutoConfigPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
