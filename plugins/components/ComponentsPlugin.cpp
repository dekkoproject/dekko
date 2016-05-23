#include "ComponentsPlugin.h"

#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include "ImageHelper.h"
#include "StretchColumn.h"
#include "StretchRow.h"
#include "Stretcher.h"
#include "LogRecorder.h"
#include "Introspector.h"
#include "PixelPerfectItem.h"
#include <Paths.h>

void ComponentsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Dekko.Components"));
    // @uri Dekko.Components
    qmlRegisterType<ImageHelper>(uri, 1, 0, "ImageHelper");
    qmlRegisterType<StretchColumn>(uri, 1, 0, "StretchColumn");
    qmlRegisterType<StretchRow>(uri, 1, 0, "StretchRow");
    qmlRegisterType<Stretcher>(uri, 1, 0, "Stretcher");
    qmlRegisterType<PixelPerfectItem>(uri, 1, 0, "PixelPerfectItem");
    qmlRegisterUncreatableType<Paths>(uri, 1, 0, "Icons", "Cannot be created in QML. THis is just for access the Icons enums");
    qmlRegisterSingletonType<Paths>(uri, 1, 0, "Paths", Paths::factory);
    qmlRegisterSingletonType<LogRecorder>(uri, 1, 0, "LogRecorder", LogRecorder::factory);
    qmlRegisterSingletonType<Introspector>(uri, 1, 0, "Introspector", Introspector::qmlSingletonProvider);
}

void ComponentsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
