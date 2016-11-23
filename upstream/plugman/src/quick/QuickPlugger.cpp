#include "QuickPlugger.h"
#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include "ActionRegistry.h"
#include "ItemRegistry.h"
#include "ListenerRegistry.h"
#include "ServiceRegistry.h"


void QuickPlugger::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Plugger"));
    // @uri Plugger
    qmlRegisterType<ActionRegistry>(uri, 1, 0, "ActionRegistry");
    qmlRegisterType<ItemRegistry>(uri, 1, 0, "ItemRegistry");
    qmlRegisterType<ListenerRegistry>(uri, 1, 0, "ListenerRegistry");
    qmlRegisterType<ServiceRegistry>(uri, 1, 0, "ServiceRegistry");
}

void QuickPlugger::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
