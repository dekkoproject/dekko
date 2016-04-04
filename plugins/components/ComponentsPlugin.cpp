#include "ComponentsPlugin.h"

#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include "ImageHelper.h"
#include <Paths.h>

void ComponentsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Dekko.Components"));
    // @uri Dekko.Components
    qmlRegisterType<ImageHelper>(uri, 1, 0, "ImageHelper");
    qmlRegisterUncreatableType<Paths>(uri, 1, 0, "Icons", "Cannot be created in QML. THis is just for access the Icons enums");
    qmlRegisterSingletonType<Paths>(uri, 1, 0, "Paths", Paths::factory);
}

void ComponentsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
