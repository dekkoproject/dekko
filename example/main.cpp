#include <QGuiApplication>
#include <PluginRegistry.h>
#include <PluginInfo.h>
#include <QScopedPointer>
#include <QDebug>
#include <QQuickView>
#include <ActionRegistry.h>
#include <ServiceRegistry.h>
#include <ItemRegistry.h>
#include "ListenerRegistry.h"

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    QCoreApplication::setApplicationName("pluggy");
    QCoreApplication::setOrganizationName("dekko");

    // First declare the plugin locations we support
    PluginRegistry::instance()->setPluginLocations(QStringList()
                << QStringLiteral("Composer::ToolbarAction")
                << QStringLiteral("View::Item")
                << QStringLiteral("Dekko::Service")
                << QStringLiteral("Dekko::Listener"));

    // Now load all plugins from supported locations, by passing
    // a list of directories. These can be anywhere you like.
    // By default the registry will always look for a plugins directory in the same location
    // as the executable i.e QCoreApplication::applicationDirPath() + "/plugins".
    // Or alternatively you can also set the DEKKO_PLUGINS environment vairable to a directory
    // that the registry should search.
    PluginRegistry::instance()->loadPlugins(QStringList() << QStringLiteral("/tmp/Dekko"));


    // Now start any non UI service plugins. These should be run as a qprocess if possibl
    // but the ServicePlugin just exposes a start() and stop() method so it can be in this
    // process if required.
    QScopedPointer<ServiceRegistry> services(new ServiceRegistry);
    services->setServiceKey(QStringLiteral("Dekko::Service"));
    services->startServices();

    qmlRegisterType<ActionRegistry>("Plugins", 0, 1, "ActionRegistry");
    qmlRegisterType<ItemRegistry>("Plugins", 0, 1, "ItemRegistry");
    qmlRegisterType<ListenerRegistry>("Plugins", 0, 1, "ListenerRegistry");

    QScopedPointer<QQuickView> view(new QQuickView);
    view->setSource(QUrl(QString("qrc:/qml/Main.qml")));
    view->show();

    return a.exec();
}
