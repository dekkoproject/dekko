#include "quickflux_plugin.h"
#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include "quickflux/qfappdispatcher.h"
#include "quickflux/qfapplistener.h"
#include "quickflux/qfappscript.h"
#include "quickflux/qfapplistenergroup.h"
#include "quickflux/qfappscriptgroup.h"
#include "quickflux/priv/qfappscriptrunnable.h"
#include "quickflux/qffilter.h"
#include "quickflux/qfkeytable.h"
#include "quickflux/qfactioncreator.h"

static QObject *appDispatcherProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    QFAppDispatcher* object = new QFAppDispatcher();
    object->setEngine(engine);

    return object;
}

void QuickFluxPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("QuickFlux"));
    // @uri QuickFlux
    qmlRegisterSingletonType<QFAppDispatcher>("QuickFlux", 1, 0, "AppDispatcher", appDispatcherProvider);
    qmlRegisterType<QFAppListener>("QuickFlux", 1, 0, "AppListener");
    qmlRegisterType<QFAppScript>("QuickFlux", 1, 0, "AppScript");
    qmlRegisterType<QFAppListenerGroup>("QuickFlux", 1, 0, "AppListenerGroup");
    qmlRegisterType<QFAppScriptGroup>("QuickFlux", 1, 0, "AppScriptGroup");
    qmlRegisterType<QFAppScriptRunnable>();
    qmlRegisterType<QFFilter>("QuickFlux", 1, 0, "Filter");
    qmlRegisterType<QFKeyTable>("QuickFlux", 1, 0, "KeyTable");
    qmlRegisterType<QFActionCreator>("QuickFlux", 1, 0, "ActionCreator");
}

void QuickFluxPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
