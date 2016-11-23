#ifndef QUICKFLUX_PLUGIN_H
#define QUICKFLUX_PLUGIN_H

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlExtensionPlugin>

class QuickFluxPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine *engine, const char *uri);
};

#endif // QUICKFLUX_PLUGIN_H
