#include "ControlsPlugin.h"

#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include "QQuickStack.h"

void ControlsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Dekko.Controls"));
    // @uri Dekko.Controls
    qmlRegisterUncreatableType<QQuickStack>(uri, 1, 0, "Stack", "Cannot be created in QML.");
}

void ControlsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
