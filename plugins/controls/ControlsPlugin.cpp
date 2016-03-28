#include "ControlsPlugin.h"

#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include "QQuickStack.h"

void ControlsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Dekko.Components"));
    // @uri Dekko.Controls
//    qmlRegisterType<ImageHelper>(uri, 1, 0, "ImageHelper");
    qmlRegisterUncreatableType<QQuickStack>(uri, 1, 0, "Stack", "Cannot be created in QML.");
//    qmlRegisterSingletonType<Paths>(uri, 1, 0, "Paths", Paths::factory);
}

void ControlsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
