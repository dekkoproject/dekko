#ifndef QUICKPLUGGER_H
#define QUICKPLUGGER_H

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlExtensionPlugin>

class QuickPlugger : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine *engine, const char *uri);
};

#endif // QUICKPLUGGER_H
